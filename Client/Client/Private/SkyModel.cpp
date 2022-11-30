#include "stdafx.h"
#include "..\Public\SkyModel.h"
#include "GameInstance.h"

#include "MapManager.h"
#include "ToolManager.h"

CSkyModel::CSkyModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));

}

CSkyModel::CSkyModel(const CSkyModel & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));

}


HRESULT CSkyModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkyModel::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CSkyModel::Tick(_float fTimeDelta)
{

}

void CSkyModel::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pGameInstance->Get_CamPosition()));
	RELEASE_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSkyModel::Render()
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



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;
	}



	return S_OK;
}



HRESULT CSkyModel::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sky"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

CSkyModel * CSkyModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkyModel*		pInstance = new CSkyModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSkyModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkyModel::Clone(void * pArg)
{
	CSkyModel*		pInstance = new CSkyModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSkyModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyModel::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
