#include "stdafx.h"
#include "..\Public\Toilet_Scream.h"
#include "GameInstance.h"

#include "MapManager.h"
#include "ToolManager.h"

CToilet_Scream::CToilet_Scream(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CToilet_Scream::CToilet_Scream(const CToilet_Scream & rhs)
	: CGameObject(rhs)
{

}


HRESULT CToilet_Scream::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToilet_Scream::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	SCREAMDESC* pDesc = (SCREAMDESC*)pArg;
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), pDesc->vRotation.x
		, XMVectorSet(0.f, 1.f, 0.f, 0.f), pDesc->vRotation.y
		, XMVectorSet(0.f, 0.f, 1.f, 0.f), pDesc->vRotation.z);

	m_sTag = "Tag_Bg";
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-60.57f, -1.f, -115.52f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(0.8f, 0.8f, 0.8f, 1.f));

	return S_OK;
}

void CToilet_Scream::Tick(_float fTimeDelta)
{

}

void CToilet_Scream::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);


	if (nullptr == m_pRendererCom)
		return;

	m_fTime += 1.f * fTimeDelta;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

HRESULT CToilet_Scream::Render()
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


	if (FAILED(m_pShaderCom->Set_RawValue("g_fTime", &m_fTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTexture_Eye->Set_SRV(m_pShaderCom, "g_Texture_Eye", 0)))
		return E_FAIL;
	if (FAILED(m_pTexture_Noise->Set_SRV(m_pShaderCom, "g_Texture_Noise", 0)))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (0 == i)
			continue;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 8)))
			return E_FAIL;
	}



	return S_OK;
}



HRESULT CToilet_Scream::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Toilet_Scream"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PipeScream_faceloop"), TEXT("Com_Texture_1"), (CComponent**)&m_pTexture_Eye)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_Noise01"), TEXT("Com_Texture_2"), (CComponent**)&m_pTexture_Noise)))
		return E_FAIL;


	return S_OK;
}

CToilet_Scream * CToilet_Scream::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CToilet_Scream*		pInstance = new CToilet_Scream(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToilet_Scream"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CToilet_Scream::Clone(void * pArg)
{
	CToilet_Scream*		pInstance = new CToilet_Scream(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToilet_Scream"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToilet_Scream::Free()
{
	__super::Free();

	Safe_Release(m_pTexture_Eye);
	Safe_Release(m_pTexture_Noise);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
