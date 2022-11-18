#include "stdafx.h"
#include "..\Public\SwipsSky_Boss.h"
#include "GameInstance.h"

#include "GameManager.h"
#include "ToolManager.h"
#include "Swip.h"

CSwipsSky_Boss::CSwipsSky_Boss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSwipsSky_Boss::CSwipsSky_Boss(const CSwipsSky_Boss & rhs)
	: CGameObject(rhs)
{
}






HRESULT CSwipsSky_Boss::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSwipsSky_Boss::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;


	SWIPSSKYDESC* pDesc = (SWIPSSKYDESC*)pArg;
	m_pOwner = pDesc->pOwner;

	m_sTag = "SwipsSky";

	CTransform* pTran = (CTransform*)m_pOwner->Get_ComponentPtr(TEXT("Com_Transform"));


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTran->Get_State(CTransform::STATE_POSITION));


	return S_OK;
}










void CSwipsSky_Boss::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	CTransform* pTran = (CTransform*)m_pOwner->Get_ComponentPtr(TEXT("Com_Transform"));
	CSwip* pBellMount = (CSwip*)m_pOwner;
	_float fRatio = pBellMount->Get_Ratio();

	fRatio *= 0.0313f;

	if (0.001f >= fRatio)
		fRatio = 0.001f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(pTran->Get_State(CTransform::STATE_POSITION), 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(fRatio, fRatio, fRatio, 1.f));


}



void CSwipsSky_Boss::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;

	CSwip* pBellMount = (CSwip*)m_pOwner;




	if (CSwip::STATE_IDLE != pBellMount->Get_State())
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}
}

HRESULT CSwipsSky_Boss::Render()
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



	return S_OK;
}

void CSwipsSky_Boss::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
}














HRESULT CSwipsSky_Boss::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("SubconSky"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;






	return S_OK;
}










CSwipsSky_Boss * CSwipsSky_Boss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSwipsSky_Boss*		pInstance = new CSwipsSky_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSwipsSky_Boss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSwipsSky_Boss::Clone(void * pArg)
{
	CSwipsSky_Boss*		pInstance = new CSwipsSky_Boss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSwipsSky_Boss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwipsSky_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
