#include "stdafx.h"
#include "..\Public\BellMountEye.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "DataManager.h"
#include "GameManager.h"
#include "CutSceneManager.h"

CBellMountEye::CBellMountEye(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CBellMountEye::CBellMountEye(const CBellMountEye & rhs)
	: CMonster(rhs)
{
}






HRESULT CBellMountEye::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBellMountEye::Initialize(void * pArg)
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


	m_iNaviIndex = CToolManager::Get_Instance()->Find_NaviIndex(XMLoadFloat3(&pDesc->vPos));

	m_eState = STATE_NONE;
	m_fAlpa = 0.f;

	m_fSpeed = 1.f;
	

	return S_OK;
}










void CBellMountEye::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);


	switch (m_eState)
	{
	case Client::CBellMountEye::STATE_NONE:
	{
		m_pModelCom->Set_AnimIndex(1);
		if(CCutSceneManager::CUT_EYE == CCutSceneManager::Get_Instance()->Get_CurCutScene())
			m_eState = STATE_APP;
	}
	break;
	case Client::CBellMountEye::STATE_APP:
	{
		m_pModelCom->Set_AnimIndex(1);

		CCutSceneManager::Get_Instance()->Add_Obj(this, 1);

		if (!m_bStartDisApear)
			return;

		m_fAlpa += fTimeDelta;

		if (1.f < m_fAlpa)
		{
			m_fAlpa = 1.f;
			m_eState = STATE_IDLE;
		}
	}
	break;
	case Client::CBellMountEye::STATE_IDLE:
	{
		m_pModelCom->Set_AnimIndex(1);
	}
	break;
	case Client::CBellMountEye::STATE_RING_UP:
	{
		if (m_fMaxRatio < m_fRatio)
		{
			m_fRatio = m_fMaxRatio;
			m_eState = STATE_RING_DOWN;
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CGameManager::Get_Instance()->Set_Wisp(false, m_fRatio, vPos);
		}
		else
		{
			m_fRatio += fTimeDelta * 10.f;
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CGameManager::Get_Instance()->Set_Wisp(false, m_fRatio, vPos);
		}

	}
	break;
	case Client::CBellMountEye::STATE_RING_DOWN:
	{
		if (0.f > m_fRatio)
		{
			m_fRatio = 0.f;
			CGameManager::Get_Instance()->Set_Wisp(false, 0, _float3(0.f, 0.f, 0.f));
			m_eState = STATE_IDLE;
		}
		else
		{
			m_fRatio -= fTimeDelta * m_fSpeed;
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CGameManager::Get_Instance()->Set_Wisp(false, m_fRatio, vPos);
		}

	}
	break;
	}



}



void CBellMountEye::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);

	if (nullptr == m_pRendererCom)
		return;




	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld);



	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		if (STATE_RING_UP == m_eState || STATE_RING_DOWN == m_eState)
			m_pModelCom->Set_AnimIndex(1);
	}



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (STATE_APP != m_eState && STATE_NONE != m_eState)
		pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);

	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	if (true == isDraw)
	{
		if (STATE_APP == m_eState || STATE_NONE == m_eState)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
			Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}
		else
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}


	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CBellMountEye::Render()
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


	_uint iPassIndex = 0;
	if (STATE_APP == m_eState || STATE_NONE == m_eState)
	{
		iPassIndex = 3;
		if (FAILED(m_pShaderCom->Set_RawValue("g_AlpaValue", &m_fAlpa, sizeof(_float))))
			return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}


	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();


	return S_OK;
}

void CBellMountEye::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{


}

void CBellMountEye::Attacked(_int iAT)
{
	m_pModelCom->Set_AnimIndex(0, true);


	m_eState = STATE_RING_UP;

	// 세이브 포인트
	_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	CGameManager::Get_Instance()->Set_SavePoint(m_iNaviIndex, vPos);

}














void CBellMountEye::Set_AnimLinearData(ANIM_LINEAR_DATA LinearData)
{
	m_pModelCom->Push_AnimLinearData(LinearData);
}

HRESULT CBellMountEye::Ready_Components()
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










CBellMountEye * CBellMountEye::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBellMountEye*		pInstance = new CBellMountEye(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBellMountEye"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBellMountEye::Clone(void * pArg)
{
	CBellMountEye*		pInstance = new CBellMountEye(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBellMountEye"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBellMountEye::Free()
{
	__super::Free();


}
