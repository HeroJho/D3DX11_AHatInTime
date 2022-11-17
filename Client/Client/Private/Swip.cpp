#include "stdafx.h"
#include "..\Public\Swip.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "DataManager.h"
#include "GameManager.h"
#include "CutSceneManager.h"

#include "Player.h"

CSwip::CSwip(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSwip::CSwip(const CSwip & rhs)
	: CGameObject(rhs)
{
}






HRESULT CSwip::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSwip::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Wisp";

	WISPDESC* pDesc = (WISPDESC*)pArg;


	m_pOwner = pDesc->pOwner;

	m_fRatio = 0.f;
	m_fMaxRatio = pDesc->fMaxRatio;
	m_fSpeed = m_fOriSpeed = pDesc->fSpeed;



	m_eState = STATE_IDLE;


	return S_OK;
}




void CSwip::Start()
{
	m_eState = STATE_RING_UP;
}





void CSwip::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	CTransform* pTran = (CTransform*)m_pOwner->Get_ComponentPtr(TEXT("Com_Transform"));
	_vector vPos = pTran->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr(LEVEL_STATIC, TEXT("Layer_Player"), 0);
	RELEASE_INSTANCE(CGameInstance);

	if (pPlayer->Get_Attacked())
		m_fSpeed = 20.f;
	else
		m_fSpeed = m_fOriSpeed;

	switch (m_eState)
	{
	case Client::CSwip::STATE_IDLE:
	{

	}
	break;
	case Client::CSwip::STATE_RING_UP:
	{
		if (m_fMaxRatio < m_fRatio)
		{
			m_fRatio = m_fMaxRatio;
			m_eState = STATE_RING_DOWN;
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CGameManager::Get_Instance()->Set_Wisp(true, m_fRatio, vPos, 0);
		}
		else
		{
			m_fRatio += fTimeDelta * 20.f;
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CGameManager::Get_Instance()->Set_Wisp(true, m_fRatio, vPos, 0);
		}

	}
	break;
	case Client::CSwip::STATE_RING_DOWN:
	{
		if (0.f > m_fRatio)
		{
			m_fRatio = 0.f;
			CGameManager::Get_Instance()->Set_Wisp(true, 0, _float3(0.f, 0.f, 0.f), 0);
			m_eState = STATE_IDLE;
		}
		else
		{
			m_fRatio -= fTimeDelta * m_fSpeed;
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CGameManager::Get_Instance()->Set_Wisp(true, m_fRatio, vPos, 0);
		}

	}
	break;
	}



}



void CSwip::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;





	// m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CSwip::Render()
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



HRESULT CSwip::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	///* For.Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("BellMount"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;



	return S_OK;
}










CSwip * CSwip::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSwip*		pInstance = new CSwip(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSwip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSwip::Clone(void * pArg)
{
	CSwip*		pInstance = new CSwip(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSwip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwip::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

}
