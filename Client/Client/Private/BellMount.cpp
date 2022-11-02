#include "stdafx.h"
#include "..\Public\BellMount.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "DataManager.h"
#include "GameManager.h"

CBellMount::CBellMount(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CBellMount::CBellMount(const CBellMount & rhs)
	: CMonster(rhs)
{
}






HRESULT CBellMount::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBellMount::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Monster";

	list<ANIM_LINEAR_DATA> LinearDatas = CDataManager::Get_Instance()->Load_Anim("BellMount");
	for (auto& Data : LinearDatas)
	{
		Set_AnimLinearData(Data);
	}


	WISPDESC* pDesc = (WISPDESC*)pArg;

	m_fRatio = 0.f;
	m_fMaxRatio = pDesc->fRatio;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Set_CurSpeed(0.f);

	m_eState = STATE_IDLE;

	return S_OK;
}










void CBellMount::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	switch (m_eState)
	{
	case Client::CBellMount::STATE_IDLE:
		{
			m_pModelCom->Set_AnimIndex(1);
		}
		break;
	case Client::CBellMount::STATE_RING_UP:
		{
			if (m_fMaxRatio < m_fRatio)
			{
				m_fRatio = m_fMaxRatio;
				m_eState = STATE_RING_DOWN;
			}
			else
			{
				m_fRatio += fTimeDelta * 20.f;
				_float3 vPos;
				XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				CGameManager::Get_Instance()->Set_Wisp(true, m_fRatio, vPos);
			}

		}
		break;
	case Client::CBellMount::STATE_RING_DOWN:
		{
			if (0.f > m_fRatio)
			{
				m_fRatio = 0.f;
				CGameManager::Get_Instance()->Set_Wisp(false, 0, _float3(0.f, 0.f, 0.f));
				m_eState = STATE_IDLE;
			}
			else
			{
				m_fRatio -= fTimeDelta;
				_float3 vPos;
				XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				CGameManager::Get_Instance()->Set_Wisp(true, m_fRatio, vPos);
			}

		}
		break;
	}



}



void CBellMount::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);

	if (nullptr == m_pRendererCom)
		return;




	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld);



	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		if(STATE_RING_UP == m_eState || STATE_RING_DOWN == m_eState)
			m_pModelCom->Set_AnimIndex(1);
	}




	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);


	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	RELEASE_INSTANCE(CGameInstance);

	if (true == isDraw)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

}

HRESULT CBellMount::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}


	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();


	return S_OK;
}

void CBellMount::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{


}

void CBellMount::Attacked(_int iAT)
{
	m_pModelCom->Set_AnimIndex(0, true);
	m_eState = STATE_RING_UP;
}
 













void CBellMount::Set_AnimLinearData(ANIM_LINEAR_DATA LinearData)
{
	m_pModelCom->Push_AnimLinearData(LinearData);
}

HRESULT CBellMount::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("BellMount"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	strcpy(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;





	return S_OK;
}










CBellMount * CBellMount::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBellMount*		pInstance = new CBellMount(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBellMount"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBellMount::Clone(void * pArg)
{
	CBellMount*		pInstance = new CBellMount(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBellMount"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBellMount::Free()
{
	__super::Free();


}
