#include "stdafx.h"
#include "..\Public\FinLaser.h"
#include "GameInstance.h"

#include "MapManager.h"
#include "ToolManager.h"


CFinLaser::CFinLaser(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CFinLaser::CFinLaser(const CFinLaser & rhs)
	: CGameObject(rhs)
{

}




HRESULT CFinLaser::Initialize_Prototype()
{



	return S_OK;
}

HRESULT CFinLaser::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;


	LASERDESC* pDesc = (LASERDESC*)pArg;

	_float fX = CToolManager::Get_Instance()->Get_RendomNum(0.f, 360.f);
	_float fY = CToolManager::Get_Instance()->Get_RendomNum(0.f, 360.f);
	_float fZ = CToolManager::Get_Instance()->Get_RendomNum(0.f, 360.f);

	_float fS = CToolManager::Get_Instance()->Get_RendomNum(0.05f, .3f);

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), fX, XMVectorSet(0.f, 1.f, 0.f, 0.f), fY, XMVectorSet(0.f, 0.f, 1.f, 0.f), fZ);
	m_fMaxScale = fS;
	m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 4.f, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z, 1.f));

	return S_OK;
}








void CFinLaser::Tick(_float fTimeDelta)
{

	_float3 vScale = m_pTransformCom->Get_Scale();
	vScale.x += m_fScaleSpeed * fTimeDelta;

	if (m_fMaxScale < vScale.x)
		vScale.x = m_fMaxScale;

	m_pTransformCom->Set_Scale(XMVectorSet(vScale.x, vScale.x , vScale.z, 1.f));

}









void CFinLaser::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}







HRESULT CFinLaser::Render()
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


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 16)))
			return E_FAIL;
	}



	return S_OK;
}







HRESULT CFinLaser::Ready_Components()
{

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return S_OK;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Laser"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	return S_OK;
}






CFinLaser * CFinLaser::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFinLaser*		pInstance = new CFinLaser(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFinLaser"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFinLaser::Clone(void * pArg)
{
	CFinLaser*		pInstance = new CFinLaser(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFinLaser"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFinLaser::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}


