#include "stdafx.h"
#include "..\Public\PuzzleCube.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "GameManager.h"
#include "ParticleManager.h"

#include "Player.h"


CPuzzleCube::CPuzzleCube(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPuzzleCube::CPuzzleCube(const CPuzzleCube & rhs)
	: CGameObject(rhs)
{
}






HRESULT CPuzzleCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPuzzleCube::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_PuzzleCube";

	PUZZLECUBEDESC* pDesc = (PUZZLECUBEDESC*)pArg;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Set_CurSpeed(0.f);


	_uint iNaviIndex = CToolManager::Get_Instance()->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	m_eState = STATE_OUT;

	return S_OK;
}







void CPuzzleCube::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);


	if (m_bInitDrop)
		return;
	m_fUpOnTimeAcc += fTimeDelta;
	if (5.f < m_fUpOnTimeAcc)
		m_bInitDrop = true;


	switch (m_eState)
	{
	case Client::CPuzzleCube::STATE_IN:
		Tick_In(fTimeDelta);
		break;
	case Client::CPuzzleCube::STATE_OUT:
		Tick_Out(fTimeDelta);
		break;
	}

}

void CPuzzleCube::Tick_In(_float fTimeDelta)
{

}

void CPuzzleCube::Tick_Out(_float fTimeDelta)
{

}








void CPuzzleCube::LateTick(_float fTimeDelta)
{
	if (!m_bInitDrop)
		return;

	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	// 중력 적용
	_bool bisIsWisp = CGameManager::Get_Instance()->Check_IsInWispWithOutSubCon(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if(bisIsWisp)
		m_eState = STATE_IN;
	else
		m_eState = STATE_OUT;
	
	if (STATE_IN != m_eState)
	{
		if (0.2f > fTimeDelta)
			m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 3.f);
	}



	bisIsWisp = CGameManager::Get_Instance()->Check_IsInWisp(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	// 여기서 셀에 있는 콜라이더와 충돌처리 확인하고 밀린다.
	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld, m_pNavigationCom, m_pTransformCom, 0.f, bisIsWisp);
	
	_float fTemp = 0.f;
	if (m_pNavigationCom->isGround(m_pTransformCom->Get_State(CTransform::STATE_POSITION), &fTemp, 0.f) || COBB::COL_ON == Get_StaticOBB()->Get_ColState())
	{
		m_bPreOn = m_bOn;
		m_bOn = true;
	}

	else
	{
		m_bPreOn = m_bOn;
		m_bOn = false;
	}


	if (m_bOn && !m_bPreOn)
	{
		_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 0.3f;
		CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(90.f, 0.f, 0.f), 0.1f, 4.f, false, 0.f, 0.f, 2.f,
			20, 0.f, 0.5f, 0.f, 0.f, 0.f, 0.1f, 0.f, 0.1f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(0.f, 360.f, 0.f), CParticle::TYPE_MODLE);
	}
	


	// 어디서든 무조건 충돌
	if (bisIsWisp)
		Get_StaticOBB()->Set_IsWall(true);
	else
		Get_StaticOBB()->Set_IsWall(false);



	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_EM, this);



	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	if (true == isDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);


	RELEASE_INSTANCE(CGameInstance);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

HRESULT CPuzzleCube::Render()
{
	if (nullptr == m_pIceModelCom ||
		nullptr == m_pWoodModelCom ||
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



	_bool bBlur = true;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bBlur", &bBlur, sizeof(_bool))))
		return E_FAIL;


	_uint		iWoodNumMeshes = m_pWoodModelCom->Get_NumMeshes();
	_uint		iIceNumMeshes = m_pIceModelCom->Get_NumMeshes();

	_uint iPassIndex = 0;
	_int iWispNum = CGameManager::Get_Instance()->Get_WispInfoNum();

	if (iWispNum)
	{
		iPassIndex = 4;

		_bool* pIsDeleteSubCons = CGameManager::Get_Instance()->Get_DeleteSubCons();
		_float* pWispRatios = CGameManager::Get_Instance()->Get_WispRatios();
		_float4* pWispPoss = CGameManager::Get_Instance()->Get_WispPoss();


		_bool bWall = false;

		if (FAILED(m_pShaderCom->Set_RawValue("g_WispInfoNum", &iWispNum, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_gIsDeleteSubCons", pIsDeleteSubCons, 256, 1)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_WispRatios", pWispRatios, 256, 2)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_WispPoss", pWispPoss, 256, 3)))
			return E_FAIL;

		bWall = false;
		if (FAILED(m_pShaderCom->Set_RawValue("g_Wall", &bWall, sizeof(_bool))))
			return E_FAIL;

		for (_uint i = 0; i < iWoodNumMeshes; ++i)
		{
			if (FAILED(m_pWoodModelCom->SetUp_OnShader(m_pShaderCom, m_pWoodModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pWoodModelCom->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}

		bWall = true;
		if (FAILED(m_pShaderCom->Set_RawValue("g_Wall", &bWall, sizeof(_bool))))
			return E_FAIL;

		for (_uint i = 0; i < iIceNumMeshes; ++i)
		{
			if (FAILED(m_pIceModelCom->SetUp_OnShader(m_pShaderCom, m_pIceModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pIceModelCom->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}

	}
	else
	{
		iPassIndex = 0;

		for (_uint i = 0; i < iWoodNumMeshes; ++i)
		{
			if (FAILED(m_pWoodModelCom->SetUp_OnShader(m_pShaderCom, m_pWoodModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pWoodModelCom->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}

		
	}
		


	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();


	return S_OK;
}

void CPuzzleCube::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("Attacked_Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag) && STATE_OUT == m_eState)
		{
			CTransform* pPlayerTran = (CTransform*)Desc.pOther->Get_ComponentPtr(TEXT("Com_Transform"));
			((CPlayer*)Desc.pOther)->SlowSpeed(0.2f);
			m_pTransformCom->PushMeX(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.MyDesc.vSize.x,
				pPlayerTran->Get_State(CTransform::STATE_POSITION), Desc.OtherDesc.vSize.x, m_pNavigationCom);

		}
	}

}













HRESULT CPuzzleCube::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("IceBlock"), TEXT("Com_IceModel"), (CComponent**)&m_pIceModelCom)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("WoodBlock"), TEXT("Com_WoodModel"), (CComponent**)&m_pWoodModelCom)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.77f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.55f, 1.65f, 1.65f);
	ColDesc.bIsStatic = true;
	ColDesc.bWall = true;
	strcpy(ColDesc.sTag, "StaticOBB");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;



	ColDesc.vCenter = _float3(0.f, 0.77f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.55f * 0.5f, 1.55f * 0.5f, 1.55f * 0.5f);
	strcpy(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.77f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(5.f, 5.f, 5.f);
	strcpy(ColDesc.sTag, "Near_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = m_CreatureDesc.iNaviIndex;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;



	return S_OK;
}










CPuzzleCube * CPuzzleCube::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPuzzleCube*		pInstance = new CPuzzleCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPuzzleCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPuzzleCube::Clone(void * pArg)
{
	CPuzzleCube*		pInstance = new CPuzzleCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPuzzleCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPuzzleCube::Free()
{
	__super::Free();


	Safe_Release(m_pIceModelCom);
	Safe_Release(m_pWoodModelCom);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}



