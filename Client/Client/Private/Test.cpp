#include "stdafx.h"
#include "..\Public\Test.h"
#include "GameInstance.h"

CTest::CTest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
}

CTest::CTest(const CTest & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
}

HRESULT CTest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTest::Initialize(void * pArg)
{
	//if (nullptr == pArg)
	//	return E_FAIL;

	CSockat::PARTSDESC* Desc = (CSockat::PARTSDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	//m_pTransformCom->Set_Scale(XMLoadFloat3(&Desc->vScale));

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), 0.f
		, XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f
		, XMVectorSet(0.f, 0.f, 1.f, 0.f), 0.f);

	//_vector vPos = XMLoadFloat3(&Desc->vPos);
	//vPos = XMVectorSetW(vPos, 1.f);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	return S_OK;
}

void CTest::Tick(_float fTimeDelta)
{

}

void CTest::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTest::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}

	return S_OK;
}



HRESULT CTest::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_SubCon"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CTest * CTest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTest*		pInstance = new CTest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTest::Clone(void * pArg)
{
	CTest*		pInstance = new CTest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTest::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
