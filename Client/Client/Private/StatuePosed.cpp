#include "stdafx.h"
#include "..\Public\StatuePosed.h"
#include "GameInstance.h"

#include  "ToolManager.h"
#include "DataManager.h"
#include "GameManager.h"
#include "ToolManager.h"

#include "Player.h"

CStatuePosed::CStatuePosed(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CStatuePosed::CStatuePosed(const CStatuePosed & rhs)
	: CMonster(rhs)
{
}






HRESULT CStatuePosed::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStatuePosed::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	STATUEDESC* Desc = (STATUEDESC*)pArg;
	m_pTarget = Desc->pTarget;

	m_sTag = "Tag_Monster";

	list<ANIM_LINEAR_DATA> LinearDatas = CDataManager::Get_Instance()->Load_Anim("StatuePosed");
	for (auto& Data : LinearDatas)
	{
		Set_AnimLinearData(Data);
	}


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&Desc->vPos), 1.f));
	m_pTransformCom->Set_CurSpeed(1.f);

	Set_State(STATE_IDLE);


	m_pTransformCom->Set_RealOriScale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	m_pNavigationCom->Set_NaviIndex(CToolManager::Get_Instance()->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));



	return S_OK;
}







void CStatuePosed::Set_AnimLinearData(ANIM_LINEAR_DATA LinearData)
{
	m_pModelCom->Push_AnimLinearData(LinearData);
}

void CStatuePosed::Set_State(STATE eState)
{

	if (eState != m_eState)
	{
		switch (m_eState)
		{
		case Client::CStatuePosed::STATE_IDLE_NONE:
			m_vPoss.clear();
			break;
		case Client::CStatuePosed::STATE_IDLE_NONE_T:
			m_vPoss.clear();
			m_fIdle_None_T_TimeAcc = 0.f;
			break;
		case Client::CStatuePosed::STATE_IDLE:
			break;
		case Client::CStatuePosed::STATE_IDLE_T:
			m_fIdle_T_TimeAcc = 0.f;
			m_fAddPosTimeAcc = 0.f;
			break;
		case Client::CStatuePosed::STATE_RUN:
			break;
		case Client::CStatuePosed::STATE_END:
			break;
		default:
			break;
		}
	}

	m_eState = eState;

	Set_Anim();
}

void CStatuePosed::Set_Anim()
{

	switch (m_eState)
	{
	case STATE_IDLE_NONE:
		m_pModelCom->Set_AnimIndex(1);
		break;
	case STATE_IDLE_NONE_T:
		m_pModelCom->Set_AnimIndex(1);
		break;
	case STATE_IDLE:
		m_pModelCom->Set_AnimIndex(1);
		break;
	case STATE_IDLE_T:
		m_pModelCom->Set_AnimIndex(1);
		break;
	case STATE_RUN:
		m_pModelCom->Set_AnimIndex(0, true);
		break;
	}

}



void CStatuePosed::Attacked(_int iAT)
{


}






void CStatuePosed::Tick(_float fTimeDelta)
{

	switch (m_eState)
	{
	case STATE_IDLE_NONE:
		Tick_IdleNone(fTimeDelta);
		break;
	case STATE_IDLE_NONE_T:
		Tick_IdleNone_T(fTimeDelta);
		break;
	case STATE_IDLE:
		Tick_Idle(fTimeDelta);
		break;
	case STATE_IDLE_T:
		Tick_Idle_T(fTimeDelta);
		break;
	case STATE_RUN:
		Tick_Run(fTimeDelta);
		break;
	}

}

void CStatuePosed::Tick_IdleNone(_float fTimeDelta)
{
	if (m_bIsInWisp)
		Set_State(STATE_IDLE_T);
}

void CStatuePosed::Tick_IdleNone_T(_float fTimeDelta)
{
	m_fIdle_None_T_TimeAcc += fTimeDelta;
	if (0.1f < m_fIdle_None_T_TimeAcc)
	{
		Set_State(STATE_IDLE_NONE);
		m_fIdle_None_T_TimeAcc = 0.f;
	}
}

void CStatuePosed::Tick_Idle(_float fTimeDelta)
{

	if(!m_bIsInWisp)
		Set_State(STATE_IDLE_NONE_T);
	else
		Set_State(STATE_RUN);

}

void CStatuePosed::Tick_Idle_T(_float fTimeDelta)
{	
	m_fIdle_T_TimeAcc += fTimeDelta;

	Add_PlayerPos(fTimeDelta);

	if (5.f < m_fIdle_T_TimeAcc)
	{
		Set_State(STATE_IDLE);
		m_fIdle_T_TimeAcc = 0.f;

		m_vNextPos = Pop_PlayerPos();
	}
}

void CStatuePosed::Tick_Run(_float fTimeDelta)
{
	Add_PlayerPos(fTimeDelta);

	if (!m_bIsInWisp)
		Set_State(STATE_IDLE_NONE_T);

	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vPlayerPos = ((CTransform*)m_pTarget->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	_float fCurDis = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));
	if (1.f > fCurDis)
		return;

	// N 초마다 플레이어의 위치를 얻어온다.
	_vector vPrePlayerPos = XMVectorSetW(XMLoadFloat3(&m_vNextPos), 1.f);

	_vector vDir = vPrePlayerPos - vMyPos;
	_float fDis = XMVectorGetX(XMVector3Length(vDir));
	


	if (1.f > fDis)
	{
		_float3 vTempPos  = Pop_PlayerPos();
		if (0.1f < XMVectorGetX(XMVector3Length(XMLoadFloat3(&vTempPos))))
			m_vNextPos = vTempPos;
	}



	vDir = XMVector3Normalize(vDir);
	m_pTransformCom->LookAt_ForLandObject(vPrePlayerPos);
	m_pTransformCom->Go_Dir(vDir, 3.f, fTimeDelta, m_pNavigationCom);

}



void CStatuePosed::Add_PlayerPos(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;


	m_fAddPosTimeAcc += fTimeDelta;
	if (0.1f < m_fAddPosTimeAcc)
	{
		CTransform* pTran = (CTransform*)m_pTarget->Get_ComponentPtr(TEXT("Com_Transform"));
		_float3 vPos;
		XMStoreFloat3(&vPos, pTran->Get_State(CTransform::STATE_POSITION));
		m_vPoss.push_back(vPos);

		m_fAddPosTimeAcc = 0.f;
	}
}

_float3 CStatuePosed::Pop_PlayerPos()
{
	if (m_vPoss.empty())
		return _float3{ 0.f, 0.f ,0.f };

	_float3 vPos = m_vPoss.front();
	m_vPoss.pop_front();

	return vPos;
}










void CStatuePosed::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_bIsInWisp = CGameManager::Get_Instance()->Check_IsInWisp(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	switch (m_eState)
	{
	case STATE_IDLE_NONE:
		LateTick_None(fTimeDelta);
	case STATE_IDLE:
	case STATE_RUN:
	case STATE_IDLE_NONE_T:
	case STATE_IDLE_T:
		LateTick_Anim(fTimeDelta);
		break;
	}


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	
	if (true == isDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	RELEASE_INSTANCE(CGameInstance);



}

void CStatuePosed::LateTick_None(_float fTimeDelta)
{

	m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom);

	// 여기서 셀에 있는 콜라이더와 충돌처리 확인하고 밀린다.
	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld, m_pNavigationCom, m_pTransformCom);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_EM, this);
	RELEASE_INSTANCE(CGameInstance);
}

void CStatuePosed::LateTick_Anim(_float fTimeDelta)
{

	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		// 애니메이션 끝 설정
		//if (1 == m_pModelCom->Get_CurAnimIndex())
		//{
		//	Set_State(STATE_IDLE_NONE);
		//}
	}

	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld);


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_PUSHMONSTER, this);
	RELEASE_INSTANCE(CGameInstance);
}




HRESULT CStatuePosed::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;



	switch (m_eState)
	{
	case STATE_IDLE_NONE:
		Render_None();
		break;
	case STATE_IDLE:
	case STATE_RUN:
		Render_Anim();
		break;
	case STATE_IDLE_NONE_T:
	case STATE_IDLE_T:
		Render_None();
		Render_Anim();
		break;
	}



	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();


	return S_OK;
}

void CStatuePosed::Render_None()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom_None->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return;
	if (FAILED(m_pShaderCom_None->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return;
	if (FAILED(m_pShaderCom_None->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return;

	RELEASE_INSTANCE(CGameInstance);






	_uint iPassIndex = 0;
	_int iWispNum = CGameManager::Get_Instance()->Get_WispInfoNum();

	if (CGameManager::Get_Instance()->Get_WispInfoNum())
	{
		iPassIndex = 3;

		_bool* pIsDeleteSubCons = CGameManager::Get_Instance()->Get_DeleteSubCons();
		_float* pWispRatios = CGameManager::Get_Instance()->Get_WispRatios();
		_float4* pWispPoss = CGameManager::Get_Instance()->Get_WispPoss();


		if (FAILED(m_pShaderCom_None->Set_RawValue("g_WispInfoNum", &iWispNum, sizeof(_int))))
			return;
		_bool bWall = false;
		if (FAILED(m_pShaderCom_None->Set_RawValue("g_Wall", &bWall, sizeof(_bool))))
			return;

		if (FAILED(m_pShaderCom_None->Set_RawValue("g_gIsDeleteSubCons", pIsDeleteSubCons, 256, 1)))
			return;
		if (FAILED(m_pShaderCom_None->Set_RawValue("g_WispRatios", pWispRatios, 256, 2)))
			return;
		if (FAILED(m_pShaderCom_None->Set_RawValue("g_WispPoss", pWispPoss, 256, 3)))
			return;

	}
	else
		iPassIndex = 0;













	_uint		iNumMeshes = m_pModelCom_None->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{

		if (FAILED(m_pModelCom_None->SetUp_OnShader(m_pShaderCom_None, m_pModelCom_None->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return;

		if (FAILED(m_pModelCom_None->Render(m_pShaderCom_None, i, iPassIndex)))
			return;
	}
}

void CStatuePosed::Render_Anim()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return;

	RELEASE_INSTANCE(CGameInstance);




	_uint iPassIndex = 0;
	_int iWispNum = CGameManager::Get_Instance()->Get_WispInfoNum();

	if (CGameManager::Get_Instance()->Get_WispInfoNum())
	{
		iPassIndex = 4;

		_bool* pIsDeleteSubCons = CGameManager::Get_Instance()->Get_DeleteSubCons();
		_float* pWispRatios = CGameManager::Get_Instance()->Get_WispRatios();
		_float4* pWispPoss = CGameManager::Get_Instance()->Get_WispPoss();


		if (FAILED(m_pShaderCom->Set_RawValue("g_WispInfoNum", &iWispNum, sizeof(_int))))
			return;
		_bool bWall = true;
		if (FAILED(m_pShaderCom->Set_RawValue("g_Wall", &bWall, sizeof(_bool))))
			return;

		if (FAILED(m_pShaderCom->Set_RawValue("g_gIsDeleteSubCons", pIsDeleteSubCons, 256, 1)))
			return;
		if (FAILED(m_pShaderCom->Set_RawValue("g_WispRatios", pWispRatios, 256, 2)))
			return;
		if (FAILED(m_pShaderCom->Set_RawValue("g_WispPoss", pWispPoss, 256, 3)))
			return;

	}
	else
		iPassIndex = 0;



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return;
	}
}



void CStatuePosed::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("Attacked_Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
		{
			if (STATE_IDLE_NONE == m_eState)
				return;

			CPlayer* pPlayer = (CPlayer*)Desc.pOther;

			if (CPlayer::STATE_JUMPATTACK != pPlayer->Get_State() &&
				false == pPlayer->Get_Attacked())
			{
				pPlayer->Attacked();
			}
		}
	}
	else if ("Tag_Monster" == Desc.pOther->Get_Tag())
	{
		// 몬스터 끼리는 밀어 낸다 
		if (!strcmp("Attacked_Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
		{
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vOtherPos = ((CTransform*)Desc.pOther->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
			_float fMyDis = Desc.MyDesc.vSize.x;
			_float fOtherDis = Desc.MyDesc.vSize.x;
			m_pTransformCom->PushMe(vMyPos, fMyDis, vOtherPos, fOtherDis, m_pNavigationCom);
		}
	}
}










HRESULT CStatuePosed::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader_1"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader_2"), (CComponent**)&m_pShaderCom_None)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("StatuePosed"), TEXT("Com_Model_1"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("StatuePosed_None"), TEXT("Com_Model_2"), (CComponent**)&m_pModelCom_None)))
		return E_FAIL;




	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;




	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));


	ColDesc.vCenter = _float3(0.f, 0.4f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	// ColDesc.vSize = _float3(0.3f, 0.8f, 0.3f);
	ColDesc.vSize = _float3(0.5f, 0.8f, 0.5f);
	ColDesc.bIsStatic = true;
	strcpy(ColDesc.sTag, "StaticOBB");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;


	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(.5f, .5f, .5f);
	strcpy(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;




	return S_OK;
}














CStatuePosed * CStatuePosed::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStatuePosed*		pInstance = new CStatuePosed(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStatuePosed"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStatuePosed::Clone(void * pArg)
{
	CStatuePosed*		pInstance = new CStatuePosed(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStatuePosed"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatuePosed::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom_None);
	Safe_Release(m_pShaderCom_None);

}
