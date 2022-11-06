#include "stdafx.h"
#include "..\Public\Wisp.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "DataManager.h"
#include "GameManager.h"
#include "CutSceneManager.h"

CWisp::CWisp(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CWisp::CWisp(const CWisp & rhs)
	: CGameObject(rhs)
{
}






HRESULT CWisp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWisp::Initialize(void * pArg)
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
	m_fSpeed = pDesc->fSpeed;


	m_eState = STATE_IDLE;


	return S_OK;
}




void CWisp::Start()
{
	m_eState = STATE_RING_UP;
}





void CWisp::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	CTransform* pTran = (CTransform*)m_pOwner->Get_ComponentPtr(TEXT("Com_Transform"));
	_vector vPos = pTran->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));

	switch (m_eState)
	{
	case Client::CWisp::STATE_IDLE:
	{
		
	}
	break;
	case Client::CWisp::STATE_RING_UP:
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
			m_fRatio += fTimeDelta * 20.f;
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CGameManager::Get_Instance()->Set_Wisp(false, m_fRatio, vPos);
		}

	}
	break;
	case Client::CWisp::STATE_RING_DOWN:
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



void CWisp::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;





	// m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CWisp::Render()
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



HRESULT CWisp::Ready_Components()
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










CWisp * CWisp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWisp*		pInstance = new CWisp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWisp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWisp::Clone(void * pArg)
{
	CWisp*		pInstance = new CWisp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CWisp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWisp::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

}
