#include "stdafx.h"
#include "..\Public\CaulDron.h"
#include "GameInstance.h"
#include  "ToolManager.h"



CCaulDron::CCaulDron(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCaulDron::CCaulDron(const CCaulDron & rhs)
	: CGameObject(rhs)
{
}






HRESULT CCaulDron::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCaulDron::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_sTag = "Tag_CaulDron";
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-60.57f, -1.f, -115.52f, 1.f));


	m_pModelCom->Set_AnimIndex(1);

	return S_OK;
}

void CCaulDron::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);




}











void CCaulDron::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;



	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		if (0 == m_pModelCom->Get_CurAnimIndex())
			m_pModelCom->Set_AnimIndex(1);
	}



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CCaulDron::Render()
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

void CCaulDron::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

}

void CCaulDron::Go_Bound()
{
	if (!m_bBound)
	{
		m_pModelCom->Set_AnimIndex(0);
		m_bBound = true;
	}
}







HRESULT CCaulDron::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("cauldron"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;




	return S_OK;
}










CCaulDron * CCaulDron::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCaulDron*		pInstance = new CCaulDron(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCaulDron"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCaulDron::Clone(void * pArg)
{
	CCaulDron*		pInstance = new CCaulDron(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCaulDron"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCaulDron::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
