#include "stdafx.h"
#include "..\Public\StaticModel_Instance.h"
#include "GameInstance.h"

#include "GameManager.h"


CStaticModel_Instance::CStaticModel_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CStaticModel_Instance::CStaticModel_Instance(const CStaticModel_Instance & rhs)
	: CGameObject(rhs)
{

}


HRESULT CStaticModel_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStaticModel_Instance::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	STATICMODELDESC* Desc = (STATICMODELDESC*)pArg;
	wcscpy_s(m_cModelTag, Desc->cModelTag);
	m_bWall = Desc->bWall;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CStaticModel_Instance::Tick(_float fTimeDelta)
{

}

void CStaticModel_Instance::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	// if (lstrcmp(TEXT("SubCon_Instance"), m_cModelTag))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}


}

HRESULT CStaticModel_Instance::Render()
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
	if (CGameManager::Get_Instance()->Get_WispBool())
	{
		iPassIndex = 1;

		_float3 vWispPos = CGameManager::Get_Instance()->Get_WispPos();
		_float fWispRatio = CGameManager::Get_Instance()->Get_WispRatio();

		if (FAILED(m_pShaderCom->Set_RawValue("g_WispRatio", &fWispRatio, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_WispPos", &vWispPos, sizeof(_float3))))
			return E_FAIL;

	}
	else
		iPassIndex = 0;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CStaticModel_Instance::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model_Instance"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_cModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

CStaticModel_Instance * CStaticModel_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStaticModel_Instance*		pInstance = new CStaticModel_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStaticModel_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStaticModel_Instance::Clone(void * pArg)
{
	CStaticModel_Instance*		pInstance = new CStaticModel_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStaticModel_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticModel_Instance::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
