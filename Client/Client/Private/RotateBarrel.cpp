#include "stdafx.h"
#include "..\Public\RotateBarrel.h"
#include "GameInstance.h"

#include "GameManager.h"
#include "ToolManager.h"

CRotateBarrel::CRotateBarrel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CRotateBarrel::CRotateBarrel(const CRotateBarrel & rhs)
	: CGameObject(rhs)
{
}






HRESULT CRotateBarrel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRotateBarrel::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_Desc = *((ROTATEBARRELDESC*)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Barrel";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_Desc.vPos.x, m_Desc.vPos.y, m_Desc.vPos.z, 1.f));

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_Desc.vRotation.x
		, XMVectorSet(0.f, 1.f, 0.f, 0.f), m_Desc.vRotation.y
		, XMVectorSet(0.f, 0.f, 1.f, 0.f), m_Desc.vRotation.z);

	m_pTransformCom->Set_CurSpeed(1.f);

	return S_OK;
}










void CRotateBarrel::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);


	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	m_pTransformCom->Turn(vRight, m_Desc.fSpeed, fTimeDelta);


}



void CRotateBarrel::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;


	Tick_Col(m_pTransformCom->Get_WorldMatrix(), nullptr, nullptr);



	if (CGameManager::Get_Instance()->Get_WispInfoNum())
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);
		RELEASE_INSTANCE(CGameInstance);
	}


	m_pOther = nullptr;
}

HRESULT CRotateBarrel::Render()
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
	_int iWispNum = CGameManager::Get_Instance()->Get_WispInfoNum();

	if (CGameManager::Get_Instance()->Get_WispInfoNum())
	{
		iPassIndex = 3;

		_bool* pIsDeleteSubCons = CGameManager::Get_Instance()->Get_DeleteSubCons();
		_float* pWispRatios = CGameManager::Get_Instance()->Get_WispRatios();
		_float4* pWispPoss = CGameManager::Get_Instance()->Get_WispPoss();


		if (FAILED(m_pShaderCom->Set_RawValue("g_WispInfoNum", &iWispNum, sizeof(_int))))
			return E_FAIL;
		_bool bWall = true;
		if (FAILED(m_pShaderCom->Set_RawValue("g_Wall", &bWall, sizeof(_bool))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_gIsDeleteSubCons", pIsDeleteSubCons, 256, 1)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_WispRatios", pWispRatios, 256, 2)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_WispPoss", pWispPoss, 256, 3)))
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


	// Render_Col();


	return S_OK;
}

void CRotateBarrel::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	//if ("Tag_Player" == Desc.pOther->Get_Tag() && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
	//{

	//	if (((COBB*)Get_StaticOBB())->Collision(Desc.pOther->Get_StaticOBB()))
	//		m_pOther = Desc.pOther;

	//}



}














HRESULT CRotateBarrel::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("secret_rotate_four"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.5f, 1.f, 19.f);
	ColDesc.bWall = true;
	strcpy(ColDesc.sTag, "StaticOBB");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 90.f, 0.f);
	ColDesc.vSize = _float3(2.5f, 1.f, 19.f);
	ColDesc.bWall = true;
	strcpy(ColDesc.sTag, "StaticOBB_1");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(10.f, 10.f, 10.f);
	strcpy(ColDesc.sTag, "Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;




	return S_OK;
}










CRotateBarrel * CRotateBarrel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRotateBarrel*		pInstance = new CRotateBarrel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRotateBarrel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRotateBarrel::Clone(void * pArg)
{
	CRotateBarrel*		pInstance = new CRotateBarrel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRotateBarrel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotateBarrel::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
