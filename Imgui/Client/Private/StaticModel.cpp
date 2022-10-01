#include "stdafx.h"
#include "..\Public\StaticModel.h"
#include "GameInstance.h"

#include "MapManager.h"


CStaticModel::CStaticModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(m_cModelTag, sizeof(TCHAR) * MAX_PATH);
	ZeroMemory(&m_vAxis, sizeof(_float3));

}

CStaticModel::CStaticModel(const CStaticModel & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(m_cModelTag, sizeof(TCHAR) * MAX_PATH);
	ZeroMemory(&m_vAxis, sizeof(_float3));

}


HRESULT CStaticModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStaticModel::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	STATICMODELDESC* Desc = (STATICMODELDESC*)pArg;
	wcscpy(m_cModelTag, Desc->cModelTag);


	if (FAILED(Ready_Components()))
		return E_FAIL;


	_vector vPos = XMLoadFloat3(&Desc->vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	_vector vScale = XMLoadFloat3(&Desc->vScale);
	vScale = XMVectorSetW(vScale, 1.f);
	m_pTransformCom->Set_Scale(vScale);

	m_vAxis = Desc->vAngle;
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_vAxis.x, XMVectorSet(0.f, 1.f, 0.f, 0.f), m_vAxis.y, XMVectorSet(0.f, 0.f, 1.f, 0.f), m_vAxis.z);

	return S_OK;
}

void CStaticModel::Tick(_float fTimeDelta)
{

}

void CStaticModel::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CStaticModel::Render()
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



	string sTemp2 = CMapManager::Get_Instance()->Get_PickedCreatedString();
	_bool bIsPicked = m_sModelNum == sTemp2;
	if (FAILED(m_pShaderCom->Set_RawValue("g_IsPicked", &bIsPicked, sizeof(_bool))))
		return E_FAIL;



	RELEASE_INSTANCE(CGameInstance);



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CStaticModel::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_cModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CStaticModel * CStaticModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStaticModel*		pInstance = new CStaticModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStaticModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStaticModel::Clone(void * pArg)
{
	CStaticModel*		pInstance = new CStaticModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStaticModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticModel::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
