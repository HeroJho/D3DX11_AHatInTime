#include "stdafx.h"
#include "..\Public\SubconEye.h"
#include "GameInstance.h"

#include  "ToolManager.h"
#include "GameManager.h"
#include "CutSceneManager.h"

#include "Player.h"


CSubconEye::CSubconEye(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CSubconEye::CSubconEye(const CSubconEye & rhs)
	: CMonster(rhs)
{
}






HRESULT CSubconEye::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSubconEye::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Monster";


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_CreatureDesc.vPos), 1.f));
	m_pTransformCom->Set_CurSpeed(1.f);
	// m_pTransformCom->Set_RealOriScale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}




void CSubconEye::Attacked(_int iAT)
{
	m_pTransformCom->ReSet_AttackedAnim();
	m_eState = STATE_ATTACKED;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->PlaySoundW(L"Player_Hurt.ogg", SOUND_EFFECT, g_fEffectSound);
	RELEASE_INSTANCE(CGameInstance);
}

void CSubconEye::Dead_Tick(_float fTimeDelta)
{
	CCutSceneManager::Get_Instance()->Add_Obj(this, 0);

	if (!m_bStartDisApear)
		return;

	

	m_fAlpa -= fTimeDelta;

	if (0.f > m_fAlpa)
	{
		m_fAlpa = 0.f;
		Set_Dead(true);
	}

}






void CSubconEye::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);



	switch (m_eState)
	{
	case STATE_IDLE:
	{

	}
	break;
	case STATE_ATTACKED:
	{
		if (m_pTransformCom->Tick_AttackAnim(fTimeDelta))
			m_eState = STATE_IDLE;
	}
	break;
	case STATE_DEAD:
	{
		Dead_Tick(fTimeDelta);
	}
		break;
	default:
		break;
	}




}


void CSubconEye::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);

	if (nullptr == m_pRendererCom)
		return;




	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld);



	m_pModelCom->Play_Animation(fTimeDelta);





	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);


	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	RELEASE_INSTANCE(CGameInstance);

	if (true == isDraw && !CGameManager::Get_Instance()->Get_CurIndex())
	{
		if (STATE_DEAD == m_eState)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
			Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}

		else
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}

}

HRESULT CSubconEye::Render()
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


	_bool bBlur = false;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bBlur", &bBlur, sizeof(_bool))))
		return E_FAIL;


	_uint iPassIndex = 5;
	if (STATE_DEAD == m_eState)
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

void CSubconEye::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("ChaseSphere", Desc.MyDesc.sTag))
		{
			if (CGameManager::Get_Instance()->Check_Stage_1())
			{
				CCutSceneManager::Get_Instance()->StartCutScene(CCutSceneManager::CUT_EYE);
				m_eState = STATE_DEAD;
			}

		}
	}
}














HRESULT CSubconEye::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("SubconEye"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));


	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(.5f, .5f, .5f);
	strcpy_s(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(10.f, 10.f, 10.f);
	strcpy_s(ColDesc.sTag, "ChaseSphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	return S_OK;
}












CSubconEye * CSubconEye::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSubconEye*		pInstance = new CSubconEye(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSubSpider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSubconEye::Clone(void * pArg)
{
	CSubconEye*		pInstance = new CSubconEye(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSubSpider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSubconEye::Free()
{
	__super::Free();


}
