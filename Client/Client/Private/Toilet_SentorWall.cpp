#include "stdafx.h"
#include "..\Public\Toilet_SentorWall.h"
#include "GameInstance.h"

#include "GameManager.h"
#include "ToolManager.h"

CToilet_SentorWall::CToilet_SentorWall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CToilet_SentorWall::CToilet_SentorWall(const CToilet_SentorWall & rhs)
	: CGameObject(rhs)
{
}






HRESULT CToilet_SentorWall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToilet_SentorWall::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_SentorWall";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-60.57f, .1f, -115.52f, 1.f));



	return S_OK;
}










void CToilet_SentorWall::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);


}



void CToilet_SentorWall::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;


	// Tick_Col(m_pTransformCom->Get_WorldMatrix(), nullptr, nullptr);



	// if (CGameManager::Get_Instance()->Get_WispInfoNum())
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

}


HRESULT CToilet_SentorWall::Render()
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


	_uint iPassIndex = 0;
	//_int iWispNum = CGameManager::Get_Instance()->Get_WispInfoNum();

	//if (CGameManager::Get_Instance()->Get_WispInfoNum())
	//{
	//	iPassIndex = 3;

	//	_bool* pIsDeleteSubCons = CGameManager::Get_Instance()->Get_DeleteSubCons();
	//	_float* pWispRatios = CGameManager::Get_Instance()->Get_WispRatios();
	//	_float4* pWispPoss = CGameManager::Get_Instance()->Get_WispPoss();


	//	if (FAILED(m_pShaderCom->Set_RawValue("g_WispInfoNum", &iWispNum, sizeof(_int))))
	//		return E_FAIL;
	//	_bool bWall = true;
	//	if (FAILED(m_pShaderCom->Set_RawValue("g_Wall", &bWall, sizeof(_bool))))
	//		return E_FAIL;

	//	if (FAILED(m_pShaderCom->Set_RawValue("g_gIsDeleteSubCons", pIsDeleteSubCons, 256, 1)))
	//		return E_FAIL;
	//	if (FAILED(m_pShaderCom->Set_RawValue("g_WispRatios", pWispRatios, 256, 2)))
	//		return E_FAIL;
	//	if (FAILED(m_pShaderCom->Set_RawValue("g_WispPoss", pWispPoss, 256, 3)))
	//		return E_FAIL;

	//}
	//else
	//	iPassIndex = 0;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 6)))
			return E_FAIL;
	}


	// Render_Col();


	return S_OK;
}



void CToilet_SentorWall::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	//if ("Tag_Player" == Desc.pOther->Get_Tag() && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
	//{

	//	if (((COBB*)Get_StaticOBB())->Collision(Desc.pOther->Get_StaticOBB()))
	//		m_pOther = Desc.pOther;

	//}



}














HRESULT CToilet_SentorWall::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Toilet_SentorWall"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	//ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(2.5f, 1.f, 19.f);
	//ColDesc.bWall = true;
	//strcpy(ColDesc.sTag, "StaticOBB");
	//if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
	//	return E_FAIL;






	return S_OK;
}










CToilet_SentorWall * CToilet_SentorWall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CToilet_SentorWall*		pInstance = new CToilet_SentorWall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToilet_SentorWall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CToilet_SentorWall::Clone(void * pArg)
{
	CToilet_SentorWall*		pInstance = new CToilet_SentorWall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToilet_SentorWall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToilet_SentorWall::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
