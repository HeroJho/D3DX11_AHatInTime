#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"

#include "DataManager.h"
#include "ToolManager.h"
#include "CamManager.h"
#include "ItemManager.h"
#include "GameManager.h"
#include "UIManager.h"
#include "CutSceneManager.h"
#include "ParticleManager.h"

#include "Camera_Free.h"

#include "Monster.h"

#include "Wisp.h"

#include "VSnatcher.h"

// TEST
#include "Yarn.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
	ZeroMemory(&m_vDestLook, sizeof(_float3));
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Player";

	list<ANIM_LINEAR_DATA> LinearDatas = CDataManager::Get_Instance()->Load_Anim("HatGirl");
	for (auto& Data : LinearDatas)
	{
		Set_AnimLinearData(Data);
	}


	m_eState = STATE_NONE;
	// m_TickStates.push_back(STATE_NONE);
	m_TickStates.push_back(STATE_APPEAR);

	m_fWalkSpeed = 1.f;
	m_fRunSpeed = 2.5f;
	m_fTurnSpeed = 1.f;
	m_fRotationSpeed = 3.5f;
	m_vDestLook = _float3{ 0.f, 0.f, 1.f };
	m_pTransformCom->Set_Look(XMLoadFloat3(&m_vDestLook));
	m_pTransformCom->Set_DestLook();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-68.97f, 11.04f, 121.20f, 1.f));
	// m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-60.6f, -0.031f, -90.23f, 1.f));  // Boss

	m_pTransformCom->Set_CurSpeed(m_fWalkSpeed);

	m_FaceAnimIndex[0] = 0;
	m_FaceAnimIndex[1] = 32;

	_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_int iNaviIndex = CToolManager::Get_Instance()->Find_NaviIndex(XMLoadFloat3(&vPos));
	CGameManager::Get_Instance()->Set_SavePoint(iNaviIndex, vPos);

	_uint iNavi = CToolManager::Get_Instance()->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNavi);


	return S_OK;
}





#pragma region GET_SET


void CPlayer::Set_AnimLinearData(ANIM_LINEAR_DATA LinearData)
{
	m_pModelCom->Push_AnimLinearData(LinearData);
}

_bool CPlayer::Get_IsPlayerOn()
{
	_float fTemp = 0.f;
	return 	m_pNavigationCom->isGround(m_pTransformCom->Get_State(CTransform::STATE_POSITION), &fTemp, 0.f) || COBB::COL_ON == Get_StaticOBB()->Get_ColState() || COBB::COL_SLIDE == Get_StaticOBB()->Get_ColState();
}

void CPlayer::Set_State()
{
	if (m_TickStates.empty())
		return;


	list<STATE>::iterator iter = max_element(m_TickStates.begin(), m_TickStates.end());

	
	m_ePreState = m_eState;
	m_eState = (*iter);



	// 애니메이션이 바뀔때 거쳐야하는 부분p
	if (m_ePreState != m_eState)
	{
		m_fStayTimeAcc = 0.f;

		switch (m_eState)
		{
		case STATE_SLEP:
			m_fSlepSpeed = 2.f;
			break;
		case STATE_JUMP:
		case STATE_RUNJUMP:
		case STATE_SPRINTJUMP:
		{
			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.1f, 0.1f, 0.1f), _float3(0.1f, 0.1f, 0.1f), _float3(0.f, 0.f, 0.f), _float3(90.f, 0.f, 0.f), 0.2f, 2.f, false, 0.f, 0.f, 1.f,
				10, 0.f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, _float3(0.f, 0.f, 0.f), _float3(0.f, 360.f, 0.f), CParticle::TYPE_MODLE);
		
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.3f, 0.3f, 0.3f), _float3(0.5f, 0.5f, 0.5f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.01f, 0.5f, false, 0.f, 0.f, 1.f,
				5, 0.f, 0.1f, 0.f, 0.f, 0.f, 2.f, 0.f, 0.f, 0.f, _float3(0.f, 0.f, 0.f), _float3(0.f, 360.f, 0.f), CParticle::TYPE_MODLE);
		}
			break;
		case STATE_SLIDE:
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_pTransformCom->Set_CurSpeed(m_pTransformCom->Get_CurSpeed() + 6.f);
			else
				m_pTransformCom->Set_CurSpeed(m_pTransformCom->Get_CurSpeed() + 4.f);

			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			_float3 vLook; XMStoreFloat3(&vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.f, 0.f, 0.f), vLook, _float3(0.5f, 0.5f, 0.5f), _float3(0.5f, 0.5f, 0.5f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.05f, 1.f, false, 0.f, 0.f, 2.f,
				10, 0.1f, 0.1f, 0.f, 0.f, 0.f, 0.2f, 0.f, 0.0f, 0.2f, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 360.f), CParticle::TYPE_MODLE);
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.f, 0.f, 0.f), vLook, _float3(0.5f, 0.5f, 0.5f), _float3(0.5f, 0.5f, 0.5f), _float3(0.f, 0.f, 0.f), _float3(-90.f, 0.f, 0.f), 0.1f, 2.f, false, 0.f, 0.f, 2.f,
				5, 0.f, 0.1f, 0.f, 0.f, 0.f, 0.2f, 0.f, 0.0f, 0.0f, _float3(-20.f, 0.f, -20.f), _float3(20.f, 0.f, 20.f), CParticle::TYPE_MODLE);
			CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_Star"), vPos, _float3(0.f, 0.f, 0.f), vLook, _float3(0.3f, 0.3f, 0.3f), _float3(0.3f, 0.3f, 0.3f), _float3(0.f, 0.f, 0.f), _float3(-90.f, 0.f, 0.f), 0.1f, 3.f, false, 0.f, 0.f, 2.f,
				5, 0.f, 0.1f, 0.f, 0.f, 0.f, 0.2f, 0.f, 0.0f, 0.2f, _float3(-90.f, 0.f, -90.f), _float3(90.f, 0.f, 90.f), CParticle::TYPE_TEXTURE);
		}
			break;
		case STATE_SLIDELENDING:
			m_fSlepSpeed = m_pTransformCom->Get_CurSpeed();
			break;
		case STATE_HILLDOWN:
			m_fHillDownTimeAcc = 0.f;
			m_fHillUpTimeAcc = 0.f;
			break;
		case STATE_MAGEDROW:
		{
			CItemManager::Get_Instance()->Make_Flask(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_State(CTransform::STATE_LOOK), 7.f, 5.f, m_pNavigationCom->Get_CurCellIndex());
		}
			break;
		case STATE_DOWN:
			m_pTransformCom->ResetGravity();
			m_pTransformCom->Jump(8.f);
			break;
		case STATE_DOUBLEJUMP:
		{
			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.1f, 0.1f, 0.1f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(90.f, 0.f, 0.f), 0.2f, 2.f, false, 0.f, 0.f, 0.5f,
				10, 0.f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, _float3(0.f, 0.f, 0.f), _float3(0.f, 360.f, 0.f), CParticle::TYPE_MODLE);
		}
			break;
		case STATE_JUMPLENDING:
		case STATE_JUMPATTACKRENDING:
		case STATE_RUNJUMPLENDING:
		case STATE_MAGEIDLEJUMPRENDING:
		case STATE_MAGERUNJUMPRENDING:   
		{
			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.1f, 0.1f, 0.1f), _float3(0.1f, 0.1f, 0.1f), _float3(0.f, 0.f, 0.f), _float3(90.f, 0.f, 0.f), 0.2f, 2.f, false, 0.f, 0.f, 1.f,
				10, 0.f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, _float3(0.f, 0.f, 0.f), _float3(0.f, 360.f, 0.f), CParticle::TYPE_MODLE);
		}
			break;
		default:
			m_fSlepSpeed = 0.f;
			break;
		}
	}







	// 애니 갱신
	Set_Anim();

	m_TickStates.clear();
}

void CPlayer::Set_Anim()
{
	switch (m_eState)
	{
	case STATE_APPEAR:
		m_pModelCom->Set_AnimIndex(111);
		break;
	case STATE_IDLE:
		m_pModelCom->Set_AnimIndex(113);
		break;
	case STATE_WALK:
	{
		m_pModelCom->Set_AnimIndex(192);
	}
		break;
	case STATE_RUN:
		if (STATE_SLEP == m_ePreState)
		{
			if (m_bImStop)
			{
				m_pTransformCom->TurnBack();
				XMStoreFloat3(&m_vDestLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				m_bImStop = false;
			}
			else
				m_pTransformCom->Set_DestLook();
		}
		m_pModelCom->Set_AnimIndex(193);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_SPRINT:
		m_pModelCom->Set_AnimIndex(153);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_JUMP:
		m_pModelCom->Set_AnimIndex(96);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_JUMPLENDING:
		m_pModelCom->Set_AnimIndex(103);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_RUNJUMP:
		m_pModelCom->Set_AnimIndex(94);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_RUNJUMPLENDING:
		m_pModelCom->Set_AnimIndex(104);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_SPRINTJUMP:
		m_pModelCom->Set_AnimIndex(106);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_DOUBLEJUMP:
		m_pModelCom->Set_AnimIndex(92);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_SLIDE:
		m_pModelCom->Set_AnimIndex(97);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_SLIDELENDING:
		m_pModelCom->Set_AnimIndex(101);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_ATTACK_1:
		Equip_Sockat(string("Umbrella"), SLOT_HAND);
		m_pModelCom->Set_AnimIndex(188);
		break;
	case STATE_ATTACK_2:
		m_pModelCom->Set_AnimIndex(189);
		break;
	case STATE_ATTACK_3:
		m_pModelCom->Set_AnimIndex(190);
		break;
	case STATE_JUMPATTACK:
		m_pModelCom->Set_AnimIndex(66);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_SLEP:
		m_pModelCom->Set_AnimIndex(180);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_HILLDOWN:
		m_pModelCom->Set_AnimIndex(53);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_STARTGETITEM:
		m_pModelCom->Set_AnimIndex(9);
		CToolManager::Get_Instance()->Set_WithOutPlayer(0.f);
		break;
	case STATE_IDLEGETITEM:
		m_pModelCom->Set_AnimIndex(7);
		break;
	case STATE_ENDGETITEM:
		m_pModelCom->Set_AnimIndex(8);
		break;
	case STATE_MAGEIDLE:
		m_pModelCom->Set_AnimIndex(150);
		Equip_Sockat(string("science_owlbrew_remade"), SLOT_HAND);
		break;
	case STATE_MAGERUN:
		m_pModelCom->Set_AnimIndex(61);
		Equip_Sockat(string("science_owlbrew_remade"), SLOT_HAND);
		break;
	case STATE_MAGEJUMP:
		m_pModelCom->Set_AnimIndex(96);
		Equip_Sockat(string("science_owlbrew_remade"), SLOT_HAND);
		break;
	case STATE_MAGEIDLEJUMPRENDING:
		m_pModelCom->Set_AnimIndex(103);
		Equip_Sockat(string("science_owlbrew_remade"), SLOT_HAND);
		break;
	case STATE_MAGERUNJUMPRENDING:
		m_pModelCom->Set_AnimIndex(104);
		Equip_Sockat(string("science_owlbrew_remade"), SLOT_HAND);
		break;
	case STATE_MAGEDROW:
		m_pModelCom->Set_AnimIndex(91);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("science_owlbrew_remade"));
		break;
	case STATE_ATTACKED:
		m_pModelCom->Set_AnimIndex(77);
		break;
	case STATE_DOWN:
		m_pModelCom->Set_AnimIndex(53);
		m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_TALK:
		m_pModelCom->Set_AnimIndex(113);
		break;
	}
}


#pragma endregion





#pragma region Tick


void CPlayer::Tick(_float fTimeDelta)
{
	if (CToolManager::Get_Instance()->Get_IsLoading())
		return;

	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_PLAYER);

	State_Input(fTimeDelta);

	Anim_Face(fTimeDelta);
	Check_Attacked(fTimeDelta);

	FoxMask_Tick(fTimeDelta);

	Stay_Tick(fTimeDelta);



	switch (m_eState)
	{
	case STATE_IDLE:
		Idle_Tick(fTimeDelta);
	case STATE_WALK:
	case STATE_RUN:
	case STATE_SPRINT:
		Move_Tick(fTimeDelta);
		break;
	case STATE_SLEP:
		Slep_Tick(fTimeDelta);
		break;
	case STATE_JUMP:
	case STATE_RUNJUMP:
		Jump_Tick(fTimeDelta);
		break;
	case STATE_JUMPLENDING:
		IdleRend_Tick(fTimeDelta);
		break;
	case STATE_RUNJUMPLENDING:
	case STATE_MAGEIDLEJUMPRENDING:
	case STATE_MAGERUNJUMPRENDING:
		Rend_Tick(fTimeDelta);
		break;
	case STATE_SPRINTJUMP:
		SprintJump_Tick(fTimeDelta);
		break;
	case STATE_DOUBLEJUMP:
		DoubleJump_Tick(fTimeDelta);
		break;
	case STATE_SLIDE:
		Slide_Tick(fTimeDelta);
		break;
	case STATE_SLIDELENDING:
		SlideRending_Tick(fTimeDelta);
		break;
	case STATE_ATTACK_1:
	case STATE_ATTACK_2:
		Attack_Input(fTimeDelta);
		break;
	case STATE_ATTACK_3:
		break;
	case STATE_READYATTACK:
		ReadyAttack_Input(fTimeDelta);
		break;
	case STATE_JUMPATTACK:
		JumpAttack_Tick(fTimeDelta);
		break;
	case STATE_HILLDOWN:
		HillDown_Tick(fTimeDelta);
		break;
	case STATE_STARTGETITEM:
		StartGetItem_Tick(fTimeDelta);
		break;
	case STATE_IDLEGETITEM:
		IdleGetItem_Tick(fTimeDelta);
		break;
	case STATE_ENDGETITEM:
		EndGetItem_Tick(fTimeDelta);
		break;
	case STATE_MAGEIDLE:
		MageIdle_Tick(fTimeDelta);
	case STATE_MAGERUN:
		MageRun_Tick(fTimeDelta);
		break;
	case STATE_MAGEJUMP:
		MageJump_Tick(fTimeDelta);
		break;
	case STATE_MAGEDROW:
		MageDrow_Tick(fTimeDelta);
		break;
	case STATE_ATTACKED:
		Attacked_Tick(fTimeDelta);
		break;
	case STATE_DOWN:
		Down_Tick(fTimeDelta);
		break;
	case STATE_TALK:
		Talk_Tick(fTimeDelta);
		break;
	}
}


void CPlayer::Idle_Tick(_float fTimeDelta)
{

	if (STATE_SPRINT == m_ePreState)
	{
		m_bImStop = true;
		m_TickStates.push_back(STATE_SLEP);
	}

	Idle_Input(fTimeDelta);

}

void CPlayer::Move_Tick(_float fTimeDelta)
{
	m_TickStates.push_back(STATE_IDLE);

	Move_Input(fTimeDelta);

	// 슬립했냐 안 했냐
	if (m_pTransformCom->LinearTurn(m_vDestLook, 1.f, 0.3f, fTimeDelta))
	{
		m_fSlepSpeed = 2.2f;
		m_TickStates.push_back(STATE_SLEP);
	}

}

void CPlayer::Jump_Tick(_float fTimeDelta)
{
	m_eJumpState = STATE_IDLE;


	Jump_Input(fTimeDelta);

	m_pTransformCom->LinearTurn(m_vDestLook, 0.1f, 0.3f, fTimeDelta, false);
}

void CPlayer::DoubleJump_Tick(_float fTimeDelta)
{
	m_eJumpState = STATE_IDLE;

	DoubleJump_Input(fTimeDelta);

	m_pTransformCom->LinearTurn(m_vDestLook, 0.5f, 0.3f, fTimeDelta, false);
}

void CPlayer::SprintJump_Tick(_float fTimeDelta)
{
	m_eJumpState = STATE_IDLE;

	SprintJump_Input(fTimeDelta);

	m_pTransformCom->LinearTurn(m_vDestLook, 0.5f, 0.3f, fTimeDelta, false);
}

void CPlayer::IdleRend_Tick(_float fTimeDelta)
{

	 Move_Input(fTimeDelta);

	if (m_pTransformCom->LinearTurn(m_vDestLook, 0.5f, 0.3f, fTimeDelta))
	{
		m_fSlepSpeed = 2.2f;
		m_TickStates.push_back(STATE_SLEP);
	}
}

void CPlayer::Rend_Tick(_float fTimeDelta)
{

	Rend_Input(fTimeDelta);

	if (m_pTransformCom->LinearTurn(m_vDestLook, 0.5f, 0.3f, fTimeDelta))
	{
		m_fSlepSpeed = 2.2f;
		m_TickStates.push_back(STATE_SLEP);
	}
}

void CPlayer::Slep_Tick(_float fTimeDelta)
{
	m_fSlepSpeed -= 6.f * fTimeDelta;
}

void CPlayer::Slide_Tick(_float fTimeDelta)
{
	m_eJumpState = STATE_IDLE;

	Slide_Input(fTimeDelta);

	m_pTransformCom->LinearTurn(m_vDestLook, 0.1f, 0.3f, fTimeDelta, false);
}

void CPlayer::SlideRending_Tick(_float fTimeDelta)
{
	if (0.f < m_pTransformCom->Get_CurSpeed())
		m_fSlepSpeed -= 3.f * fTimeDelta;
	else
		m_fSlepSpeed = 0.f;

	SlideRending_Input(fTimeDelta);
}

void CPlayer::HillDown_Tick(_float fTimeDelta)
{
	// 실시간으로 면의 정보를 가져와야한다.
	_float3 vPoss[3];
	ZeroMemory(&vPoss, sizeof(_float3) * 3);
	
	// 셀 없다
	if (!m_pNavigationCom->Get_GroundCell(vPoss, m_pTransformCom))
	{
		memcpy(vPoss, Get_StaticOBB()->Get_HillPoss(), sizeof(_float3) * 3);
		_vector vA = XMLoadFloat3(&vPoss[0]);
		_vector vB = XMLoadFloat3(&vPoss[1]);
		_vector vC = XMLoadFloat3(&vPoss[2]);

		if (0.001f > fabs(XMVectorGetX(XMVector3Length(vA)) - XMVectorGetX(XMVector3Length(vB))))
		{// 면 없다

			// 낙하?
			return;
		}
	}

	_vector vPlan;
	vPlan = XMPlaneFromPoints(XMLoadFloat3(&vPoss[0]), XMLoadFloat3(&vPoss[1]), XMLoadFloat3(&vPoss[2]));
	vPlan = XMVectorSetW(XMVector3Normalize(vPlan), 0.f);


	// 면을 탄다
	_vector vY = XMVectorSet(0.f, 2.f, 0.f, 0.f); // 빛
	_float vDot = XMVectorGetX(XMVector3Dot(vPlan, vY));
	_vector vNorPlan = vPlan * vDot;

	// 드러눕기
	m_fHillDownTimeAcc += fTimeDelta;
	if (1.5f < m_fHillDownTimeAcc)
	{
		// 면의 노말 벡터를 내 업 벡터로 맞춘다.

		HillDown_Input(fTimeDelta);
		m_pTransformCom->Set_Up(vPlan);
	}


	// 만약에 vDot 값이 1.99가 넘어간다면 평평한 곳에 왔다는 것
	// 잠깐 누워있다가 스페이스바로 Idle 복귀 가능
	if (1.8f < vDot)
	{
		m_fHillDownSpeed -= fTimeDelta * 0.1f;

		if (0.f > m_fHillDownSpeed)
			m_fHillDownSpeed = 0.0f;

		m_fHillUpTimeAcc += fTimeDelta;

		if(1.f < m_fHillUpTimeAcc)
			HillDown_Input(fTimeDelta);
	}
	else
	{
		XMStoreFloat3(&m_vHillDir, vNorPlan - vY);
		m_fHillDownSpeed += fTimeDelta * 0.1f;
	}

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vHillDir), m_fHillDownSpeed, fTimeDelta);

	// HillDown_Input(fTimeDelta);

}

void CPlayer::JumpAttack_Tick(_float fTimeDelta)
{
	Find_NearstMonster();

	// 몬스터의 머리 방향으로 내려 꼳는다
	CTransform* pMonsterTran = (CTransform*)m_pNearstMonster->Get_ComponentPtr(TEXT("Com_Transform"));


	_float3 vCenter = m_pNearstMonster->Get_Colliders(string("Attacked_Sphere"))->Get_Desc().vCenter;
	vCenter.y += m_pNearstMonster->Get_Colliders(string("Attacked_Sphere"))->Get_Desc().vSize.y;
	_vector vMonsterHeadPos = pMonsterTran->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&vCenter);
	m_pTransformCom->LookAt(vMonsterHeadPos);
	m_pTransformCom->Go_Straight(10.f, fTimeDelta, m_pNavigationCom);


	JumpAttack_Input(fTimeDelta);
}

void CPlayer::Attacked_Tick(_float fTimeDelta)
{
	m_fAttackedTimeAcc += fTimeDelta;

	if (1.f < m_fAttackedTimeAcc)
	{
		m_TickStates.push_back(STATE_IDLE);
		m_fAttackedTimeAcc = 0.f;
	}

}

void CPlayer::StartGetItem_Tick(_float fTimeDelta)
{
	StartGetItem_Input(fTimeDelta);

}

void CPlayer::IdleGetItem_Tick(_float fTimeDelta)
{
	IdleGetItem_Input(fTimeDelta);

}

void CPlayer::EndGetItem_Tick(_float fTimeDelta)
{
	EndGetItem_Input(fTimeDelta);

}

void CPlayer::MageIdle_Tick(_float fTimeDelta)
{
	m_fMageTimeAcc += fTimeDelta;

	MageIdle_Input(fTimeDelta);
}

void CPlayer::MageRun_Tick(_float fTimeDelta)
{
	m_fMageTimeAcc += fTimeDelta;

	MageRun_Input(fTimeDelta);

	// 슬립했냐 안 했냐
	if (m_pTransformCom->LinearTurn(m_vDestLook, 1.f, 0.3f, fTimeDelta))
	{
		m_fSlepSpeed = 2.2f;
		m_TickStates.push_back(STATE_SLEP);
	}
}

void CPlayer::MageJump_Tick(_float fTimeDelta)
{
	m_fMageTimeAcc += fTimeDelta;

	m_eJumpState = STATE_IDLE;

	MageJump_Input(fTimeDelta);

	m_pTransformCom->LinearTurn(m_vDestLook, 0.1f, 0.3f, fTimeDelta, false);
}

void CPlayer::MageDrow_Tick(_float fTimeDelta)
{
	m_fMageTimeAcc = 0.f;

	MageDrow_Input(fTimeDelta);
}

void CPlayer::Down_Tick(_float fTimeDelta)
{
	_float fTemp = 0.f;
	if(Get_IsPlayerOn())
		HillDown_Input(fTimeDelta);
}

void CPlayer::FoxMask_Tick(_float fTimeDelta)
{
	if (!m_bFoxMask)
		return;
	

	m_fFoxMaskTimeAcc += fTimeDelta;
	if (10.f < m_fFoxMaskTimeAcc || "Mask_Fox" != m_pSockatCom->Get_SlotTag(SLOT_HAT))
	{
		m_bFoxMask = false;
		m_fFoxMaskTimeAcc = 0.f;
		CToolManager::Get_Instance()->Set_All(1.f);
	}
	
}

void CPlayer::Stay_Tick(_float fTimeDelta)
{


	switch (m_eState)
	{
	case Client::CPlayer::STATE_IDLE:
		break;
	case Client::CPlayer::STATE_WALK:
	{
		m_fStayTimeAcc += fTimeDelta;

		if (0.5f < m_fStayTimeAcc)
		{
			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			_float3 vLook; XMStoreFloat3(&vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.1f, 0.1f, 0.f), vLook, _float3(0.5f, 0.5f, 0.5f), _float3(0.9f, 0.9f, 0.9f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 90.f), 0.05f, 0.f, false, 0.f, 0.f, 1.f,
				1, 0.2f, 0.1f, 0.1f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), CParticle::TYPE_MODLE);

			m_fStayTimeAcc = 0.f;
		}

	}
		break;
	case Client::CPlayer::STATE_RUN:
	{
		m_fStayTimeAcc += fTimeDelta;

		if (0.4f < m_fStayTimeAcc)
		{
			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			_float3 vLook; XMStoreFloat3(&vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.1f, 0.1f, 0.f), vLook, _float3(0.5f, 0.5f, 0.5f), _float3(0.9f, 0.9f, 0.9f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 90.f), 0.05f, 0.f, false, 0.f, 0.f, 1.f,
				1, 0.2f, 0.1f, 0.1f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), CParticle::TYPE_MODLE);

			m_fStayTimeAcc = 0.f;
		}

	}
		break;
	case Client::CPlayer::STATE_SPRINT:
	{
		m_fStayTimeAcc += fTimeDelta;

		if (0.1f < m_fStayTimeAcc)
		{
			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			_float3 vLook; XMStoreFloat3(&vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SprintParticle"), vPos, _float3(0.1f, 0.2f, 0.f), vLook, _float3(0.5f, 0.5f, 0.5f), _float3(1.6f, 1.6f, 1.6f), _float3(0.f, -90.f, 0.f), _float3(0.f, 0.f, 90.f), 0.1f, 0.f, false, 0.f, 0.f, 1.f,
				1, 0.2f, 0.2f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), CParticle::TYPE_MODLE);
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.1f, 0.2f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.6f, 0.6f, 0.6f), _float3(0.9f, 0.9f, 0.9f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 90.f), 0.05f, 0.f, false, 0.f, 0.f, 1.5f,
				3, 0.2f, 0.1f, 0.1f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(0.f, 360.f, 0.f), CParticle::TYPE_MODLE);


			m_fStayTimeAcc = 0.f;
		}
	}
		break;
	case Client::CPlayer::STATE_SLEP:
		break;
	case Client::CPlayer::STATE_JUMP:
		break;
	case Client::CPlayer::STATE_SLIDE:
	{
		m_fStayTimeAcc += fTimeDelta;

		if (0.1f < m_fStayTimeAcc)
		{
			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			_float3 vLook; XMStoreFloat3(&vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.1f, 0.1f, 0.f), vLook, _float3(0.5f, 0.5f, 0.5f), _float3(0.9f, 0.9f, 0.9f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 90.f), 0.05f, 0.f, false, 0.f, 0.f, 1.f,
				1, 0.2f, 0.1f, 0.1f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), CParticle::TYPE_MODLE);

			m_fStayTimeAcc = 0.f;
		}
	}
		break;
	case Client::CPlayer::STATE_JUMPLENDING:
		break;
	case Client::CPlayer::STATE_RUNJUMP:
		break;
	case Client::CPlayer::STATE_RUNJUMPLENDING:
	{
		m_fStayTimeAcc += fTimeDelta;

		if (0.4f < m_fStayTimeAcc)
		{
			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			_float3 vLook; XMStoreFloat3(&vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.1f, 0.1f, 0.f), vLook, _float3(0.5f, 0.5f, 0.5f), _float3(0.9f, 0.9f, 0.9f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 90.f), 0.05f, 0.f, false, 0.f, 0.f, 1.f,
				1, 0.2f, 0.1f, 0.1f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), CParticle::TYPE_MODLE);

			m_fStayTimeAcc = 0.f;
		}
	}
		break;
	case Client::CPlayer::STATE_SLIDELENDING:
	{
		m_fStayTimeAcc += fTimeDelta;

		if (0.2f < m_fStayTimeAcc && 0.1f < m_pTransformCom->Get_CurSpeed())
		{
			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			_float3 vLook; XMStoreFloat3(&vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.1f, 0.1f, 0.f), vLook, _float3(0.5f, 0.5f, 0.5f), _float3(0.9f, 0.9f, 0.9f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 90.f), 0.05f, 0.f, false, 0.f, 0.f, 1.f,
				1, 0.2f, 0.1f, 0.1f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), CParticle::TYPE_MODLE);

			m_fStayTimeAcc = 0.f;
		}
	}
		break;
	case Client::CPlayer::STATE_SPRINTJUMP:
		break;
	case Client::CPlayer::STATE_DOUBLEJUMP:
		break;
	case Client::CPlayer::STATE_ATTACK_1:
		break;
	case Client::CPlayer::STATE_ATTACK_2:
		break;
	case Client::CPlayer::STATE_ATTACK_3:
		break;
	case Client::CPlayer::STATE_READYATTACK:
		break;
	case Client::CPlayer::STATE_JUMPATTACK:
		break;
	case Client::CPlayer::STATE_JUMPATTACKRENDING:
		break;
	case Client::CPlayer::STATE_HILLDOWN:
		break;
	case Client::CPlayer::STATE_STARTGETITEM:
		break;
	case Client::CPlayer::STATE_IDLEGETITEM:
		break;
	case Client::CPlayer::STATE_ENDGETITEM:
		break;
	case Client::CPlayer::STATE_MAGEIDLE:
		break;
	case Client::CPlayer::STATE_MAGERUN:
		break;
	case Client::CPlayer::STATE_MAGEJUMP:
		break;
	case Client::CPlayer::STATE_MAGEIDLEJUMPRENDING:
		break;
	case Client::CPlayer::STATE_MAGERUNJUMPRENDING:
		break;
	case Client::CPlayer::STATE_MAGEDROW:
		break;
	case Client::CPlayer::STATE_DOWN:
		break;
	case Client::CPlayer::STATE_ATTACKED:
		break;
	case Client::CPlayer::STATE_TALK:
		break;
	case Client::CPlayer::STATE_APPEAR:
		break;
	case Client::CPlayer::STATE_NONE:
		break;
	}



}

void CPlayer::Talk_Tick(_float fTimeDelta)
{

	Talk_Input(fTimeDelta);
}



void CPlayer::State_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// TEST
	if (pGameInstance->Key_Down(DIK_1))
	{
		CToolManager::Get_Instance()->Resul_Level(LEVEL_BOSS);
		RELEASE_INSTANCE(CGameInstance);

		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	if (pGameInstance->Key_Down(DIK_N))
	{
		//_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		//CItemManager::Get_Instance()->Make_PopSprintItem(TEXT("Prototype_GameObject_Diamond"), TEXT("capsule"), LEVEL_GAMEPLAY, vPos, _float3(0.f, 0.f, 0.f), _float3(1.f, 1.f, 1.f), 1, m_pNavigationCom->Get_CurCellIndex(), 15);

	}


	if (pGameInstance->Key_Down(DIK_LSHIFT))
	{
		if ("Mask_Cat" == m_pSockatCom->Get_SlotTag(SLOT_HAT) && CWisp::STATE_IDLE == m_pWisp->Get_State())
			m_pWisp->Start();
		else if ("Mask_Fox" == m_pSockatCom->Get_SlotTag(SLOT_HAT) && !m_bFoxMask)
		{
			m_bFoxMask = true;
			m_fFoxMaskTimeAcc = 0.f;
			CToolManager::Get_Instance()->Set_WithOutPlayer(0.15f);
		}

		_float3 vDir; XMStoreFloat3(&vDir, m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		// CCamManager::Get_Instance()->Get_Cam()->Start_Shake(0.3f, 3.f, 0.05f);

	}



	RELEASE_INSTANCE(CGameInstance);
	 
}

void CPlayer::Idle_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);




	RELEASE_INSTANCE(CGameInstance);

}

void CPlayer::Move_Input(_float fTimeDelta)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// WD : 카메라 Lend Look으로 Look
	// AD : 카메라 Right로 Look
	_matrix mCamWorld = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	_vector vCamRight = mCamWorld.r[0];
	_vector vCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vLendLook = XMVector3Cross(vCamRight, vCamUp);

	vLendLook = XMVector3Normalize(vLendLook);
	vCamRight = XMVector3Normalize(vCamRight);


	_vector vTotalLook = XMLoadFloat3(&m_vDestLook);

	if (pGameInstance->Key_Pressing(DIK_W))
	{
		vTotalLook += vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_TickStates.push_back(STATE_SPRINT);
			else
				m_TickStates.push_back(STATE_RUN);
		}
		else
		{
			if ("Ori_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT) || "" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_TickStates.push_back(STATE_WALK);
			else
				m_TickStates.push_back(STATE_RUN);
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		vTotalLook += -1.f * vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_TickStates.push_back(STATE_SPRINT);
			else
				m_TickStates.push_back(STATE_RUN);
		}
		else
		{
			if ("Ori_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT) || "" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_TickStates.push_back(STATE_WALK);
			else
				m_TickStates.push_back(STATE_RUN);
		}
	}


	if (pGameInstance->Key_Pressing(DIK_A))
	{
		vTotalLook += -1.f * vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_TickStates.push_back(STATE_SPRINT);
			else
				m_TickStates.push_back(STATE_RUN);
		}
		else
		{
			if ("Ori_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT) || "" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_TickStates.push_back(STATE_WALK);
			else
				m_TickStates.push_back(STATE_RUN);
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		vTotalLook += vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_TickStates.push_back(STATE_SPRINT);
			else
				m_TickStates.push_back(STATE_RUN);
		}
		else
		{
			if ("Ori_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT) || "" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_TickStates.push_back(STATE_WALK);
			else
				m_TickStates.push_back(STATE_RUN);
		}
	}


	if (pGameInstance->Key_Down(DIK_SPACE))
	{

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
			{
				m_TickStates.push_back(STATE_SPRINTJUMP);
				m_pTransformCom->Jump(m_fJumpPower + 2.f);
			}
			else
			{
				m_TickStates.push_back(STATE_RUNJUMP);
				m_pTransformCom->Jump(m_fJumpPower);
			}

		}
		else
		{
			m_TickStates.push_back(STATE_JUMP);
			m_pTransformCom->Jump(m_fJumpPower);
		}

	}


	if (pGameInstance->Mouse_Down(DIMK_LBUTTON) && !pGameInstance->Key_Pressing(DIK_TAB))
	{
		m_TickStates.push_back(STATE_ATTACK_1);
	}


	if (pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		if ("Witch_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
		{
			m_TickStates.push_back(STATE_MAGEIDLE);
			m_fMageTimeAcc = 0.f;
		}
	}

	vTotalLook = XMVector3Normalize(vTotalLook);
	XMStoreFloat3(&m_vDestLook, vTotalLook);


	RELEASE_INSTANCE(CGameInstance);																																
}

void CPlayer::Attack_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (pGameInstance->Mouse_Down(DIMK_LBUTTON))
	{
		if (STATE_ATTACK_1 == m_eState)
			m_ComboStates.push_back(STATE_ATTACK_2);
		else if (STATE_ATTACK_2 == m_eState)
			m_ComboStates.push_back(STATE_ATTACK_3);
	}


	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::ReadyAttack_Input(_float fTimeDelta)
{
	if (m_ComboStates.empty())
	{
		m_TickStates.push_back(STATE_IDLE);
		// m_pSockatCom->Remove_Sockat(SLOT_HAND);
		return;
	}

	if (STATE_ATTACK_2 == m_ComboStates.front())
		m_TickStates.push_back(STATE_ATTACK_2);
	else if (STATE_ATTACK_3 == m_ComboStates.front())
		m_TickStates.push_back(STATE_ATTACK_3);
	else
		m_TickStates.push_back(STATE_IDLE);

	m_ComboStates.clear();
}

void CPlayer::Jump_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix mCamWorld = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	_vector vCamRight = mCamWorld.r[0];
	_vector vCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vLendLook = XMVector3Cross(vCamRight, vCamUp);

	vLendLook = XMVector3Normalize(vLendLook);
	vCamRight = XMVector3Normalize(vCamRight);


	_vector vTotalLook = XMLoadFloat3(&m_vDestLook);

	if (pGameInstance->Key_Pressing(DIK_W))
	{
		vTotalLook += vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_eJumpState = STATE_SPRINT;
			else
				m_eJumpState = STATE_RUN;
		}
		else
		{
			if ("Ori_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT) || "" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_eJumpState = STATE_WALK;
			else
				m_eJumpState = STATE_RUN;
		}

	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		vTotalLook += -1.f * vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_eJumpState = STATE_SPRINT;
			else
				m_eJumpState = STATE_RUN;
		}
		else
		{
			if ("Ori_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT) || "" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_eJumpState = STATE_WALK;
			else
				m_eJumpState = STATE_RUN;
		}
	}


	if (pGameInstance->Key_Pressing(DIK_A))
	{
		vTotalLook += -1.f * vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_eJumpState = STATE_SPRINT;
			else
				m_eJumpState = STATE_RUN;
		}
		else
		{
			if ("Ori_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT) || "" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_eJumpState = STATE_WALK;
			else
				m_eJumpState = STATE_RUN;
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		vTotalLook += vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_eJumpState = STATE_SPRINT;
			else
				m_eJumpState = STATE_RUN;
		}
		else
		{
			if ("Ori_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT) || "" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_eJumpState = STATE_WALK;
			else
				m_eJumpState = STATE_RUN;
		}
	}


	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		m_TickStates.push_back(STATE_DOUBLEJUMP);
		m_pTransformCom->DoubleJump(m_fJumpPower);
	}
	else if (pGameInstance->Key_Down(DIK_LCONTROL))
	{
		m_TickStates.push_back(STATE_SLIDE);
		m_pTransformCom->ResetGravity();
	}


	vTotalLook = XMVector3Normalize(vTotalLook);
	XMStoreFloat3(&m_vDestLook, vTotalLook);


	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::DoubleJump_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// WD : 카메라 Lend Look으로 Look
	// AD : 카메라 Right로 Look
	_matrix mCamWorld = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	_vector vCamRight = mCamWorld.r[0];
	_vector vCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vLendLook = XMVector3Cross(vCamRight, vCamUp);

	vLendLook = XMVector3Normalize(vLendLook);
	vCamRight = XMVector3Normalize(vCamRight);


	_vector vTotalLook = XMLoadFloat3(&m_vDestLook);

	if (pGameInstance->Key_Pressing(DIK_W))
	{
		vTotalLook += vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_eJumpState = STATE_SPRINT;
			else
				m_eJumpState = STATE_RUN;
		}
		else
			m_eJumpState = STATE_RUN;
	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		vTotalLook += -1.f * vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_eJumpState = STATE_SPRINT;
			else
				m_eJumpState = STATE_RUN;
		}
		else
			m_eJumpState = STATE_RUN;
	}


	if (pGameInstance->Key_Pressing(DIK_A))
	{
		vTotalLook += -1.f * vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_eJumpState = STATE_SPRINT;
			else
				m_eJumpState = STATE_RUN;
		}
		else
			m_eJumpState = STATE_RUN;
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		vTotalLook += vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
				m_eJumpState = STATE_SPRINT;
			else
				m_eJumpState = STATE_RUN;
		}
		else
			m_eJumpState = STATE_RUN;
	}




	// 점프 공격
	if (pGameInstance->Mouse_Down(DIMK_LBUTTON))
	{
		// 내 주변에 몬스터가 있는지 확인한다.
		// 몬스터가 있따면 GO
		if (!m_pNearMonsters.empty())
		{
			Find_NearstMonster();
			CTransform* pMonsterTran = (CTransform*)m_pNearstMonster->Get_ComponentPtr(TEXT("Com_Transform"));

			_vector vMonsterPos = pMonsterTran->Get_State(CTransform::STATE_POSITION);
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float fDis = XMVectorGetX(XMVector3Length(vMonsterPos - vMyPos));
			if (5.f > fDis)
			{
				m_TickStates.push_back(STATE_JUMPATTACK);
			}

		}

	}


	if (pGameInstance->Key_Down(DIK_LCONTROL))
	{
		m_TickStates.push_back(STATE_SLIDE);
		m_pTransformCom->ResetGravity();
	}


	vTotalLook = XMVector3Normalize(vTotalLook);
	XMStoreFloat3(&m_vDestLook, vTotalLook);


	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::SprintJump_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// WD : 카메라 Lend Look으로 Look
	// AD : 카메라 Right로 Look
	_matrix mCamWorld = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	_vector vCamRight = mCamWorld.r[0];
	_vector vCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vLendLook = XMVector3Cross(vCamRight, vCamUp);

	vLendLook = XMVector3Normalize(vLendLook);
	vCamRight = XMVector3Normalize(vCamRight);


	_vector vTotalLook = XMLoadFloat3(&m_vDestLook);

	if (pGameInstance->Key_Pressing(DIK_W))
	{
		vTotalLook += vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
			m_eJumpState = STATE_RUN;
		//else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		//	m_eJumpState = STATE_SPRINT;
		else
			m_eJumpState = STATE_RUN;
	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		vTotalLook += -1.f * vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
			m_eJumpState = STATE_RUN;
		//else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		//	m_eJumpState = STATE_SPRINT;
		else
			m_eJumpState = STATE_RUN;
	}


	if (pGameInstance->Key_Pressing(DIK_A))
	{
		vTotalLook += -1.f * vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
			m_eJumpState = STATE_RUN;
		//else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		//	m_eJumpState = STATE_SPRINT;
		else
			m_eJumpState = STATE_RUN;
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		vTotalLook += vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
			m_eJumpState = STATE_RUN;
		//else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		//	m_eJumpState = STATE_SPRINT;
		else
			m_eJumpState = STATE_RUN;
	}


	if (pGameInstance->Key_Down(DIK_LCONTROL))
	{
		m_TickStates.push_back(STATE_SLIDE);
		m_pTransformCom->ResetGravity();
	}


	vTotalLook = XMVector3Normalize(vTotalLook);
	XMStoreFloat3(&m_vDestLook, vTotalLook);


	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Rend_Input(_float fTimeDleta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// WD : 카메라 Lend Look으로 Look
	// AD : 카메라 Right로 Look
	_matrix mCamWorld = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	_vector vCamRight = mCamWorld.r[0];
	_vector vCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vLendLook = XMVector3Cross(vCamRight, vCamUp);

	vLendLook = XMVector3Normalize(vLendLook);
	vCamRight = XMVector3Normalize(vCamRight);


	_vector vTotalLook = XMLoadFloat3(&m_vDestLook);

	if (pGameInstance->Key_Pressing(DIK_W))
	{
		vTotalLook += vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vLendLook;

	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		vTotalLook += -1.f * vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vLendLook;

	}


	if (pGameInstance->Key_Pressing(DIK_A))
	{
		vTotalLook += -1.f * vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vCamRight;

	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		vTotalLook += vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vCamRight;

	}


	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pTransformCom->Jump(m_fJumpPower);

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_TickStates.push_back(STATE_RUNJUMP);
			m_pTransformCom->Set_CurSpeed(m_fRunSpeed);
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_TickStates.push_back(STATE_SPRINTJUMP);
			m_pTransformCom->Set_CurSpeed(m_fRunSpeed + 2.f);
		}
		else
		{
			if ("Ori_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HAT) || "" == m_pSockatCom->Get_SlotTag(SLOT_HAT))
			{
				m_TickStates.push_back(STATE_JUMP);
				m_pTransformCom->Set_CurSpeed(m_fWalkSpeed);
			}
			else
			{
				m_TickStates.push_back(STATE_RUNJUMP);
				m_pTransformCom->Set_CurSpeed(m_fRunSpeed);
			}
		}
	}


	vTotalLook = XMVector3Normalize(vTotalLook);
	XMStoreFloat3(&m_vDestLook, vTotalLook);


	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Slide_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		m_TickStates.push_back(STATE_DOUBLEJUMP);
		m_pTransformCom->DoubleJump(m_fJumpPower);
		m_pTransformCom->Set_CurSpeed(m_pTransformCom->Get_CurSpeed() - 4.f);
	}


	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::SlideRending_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		m_TickStates.push_back(STATE_DOUBLEJUMP);
		m_pTransformCom->DoubleJump(m_fJumpPower - 2.f);
		m_pTransformCom->Set_CurSpeed(m_pTransformCom->Get_CurSpeed() - 2.f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::HillDown_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pTransformCom->Jump(m_fJumpPower - 2.f);
		m_TickStates.push_back(STATE_DOUBLEJUMP);

		m_fHillDownTimeAcc = 0.f;
		m_fHillUpTimeAcc = 0.f;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::JumpAttack_Input(_float fTimeDelta)
{


}

void CPlayer::StartGetItem_Input(_float fTimeDelta)
{
}

void CPlayer::IdleGetItem_Input(_float fTimeDelta)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		m_TickStates.push_back(STATE_ENDGETITEM);
		m_pSockatCom->Remove_Sockat(SLOT_HAND);
		//CCamManager::Get_Instance()->End_CutScene();
	}


	RELEASE_INSTANCE(CGameInstance);


}

void CPlayer::EndGetItem_Input(_float fTimeDelta)
{
}

void CPlayer::MageIdle_Input(_float fTimeDelta)
{

}

void CPlayer::MageRun_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// WD : 카메라 Lend Look으로 Look
	// AD : 카메라 Right로 Look
	_matrix mCamWorld = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	_vector vCamRight = mCamWorld.r[0];
	_vector vCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vLendLook = XMVector3Cross(vCamRight, vCamUp);

	vLendLook = XMVector3Normalize(vLendLook);
	vCamRight = XMVector3Normalize(vCamRight);


	_vector vTotalLook = XMLoadFloat3(&m_vDestLook);



	if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		m_TickStates.push_back(STATE_MAGEIDLE);
	else
	{
		if (m_fMageGageTime < m_fMageTimeAcc)
			m_TickStates.push_back(STATE_MAGEDROW);
		else
		{
			m_TickStates.push_back(STATE_IDLE);
			m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("science_owlbrew_remade"));
		}

		RELEASE_INSTANCE(CGameInstance);
		return;
	}






	if (pGameInstance->Key_Pressing(DIK_W))
	{
		vTotalLook += vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vLendLook;

		m_TickStates.push_back(STATE_MAGERUN);

	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		vTotalLook += -1.f * vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vLendLook;

		m_TickStates.push_back(STATE_MAGERUN);
	}


	if (pGameInstance->Key_Pressing(DIK_A))
	{
		vTotalLook += -1.f * vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vCamRight;

		m_TickStates.push_back(STATE_MAGERUN);
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		vTotalLook += vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vCamRight;

		m_TickStates.push_back(STATE_MAGERUN);
	}


	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		m_TickStates.push_back(STATE_MAGEJUMP);
		m_pTransformCom->Jump(m_fJumpPower);
	}


	vTotalLook = XMVector3Normalize(vTotalLook);
	XMStoreFloat3(&m_vDestLook, vTotalLook);


	RELEASE_INSTANCE(CGameInstance);


}

void CPlayer::MageJump_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix mCamWorld = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	_vector vCamRight = mCamWorld.r[0];
	_vector vCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vLendLook = XMVector3Cross(vCamRight, vCamUp);

	vLendLook = XMVector3Normalize(vLendLook);
	vCamRight = XMVector3Normalize(vCamRight);


	_vector vTotalLook = XMLoadFloat3(&m_vDestLook);

	if (pGameInstance->Key_Pressing(DIK_W))
	{
		vTotalLook += vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vLendLook;

		m_eJumpState = STATE_RUN;
	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		vTotalLook += -1.f * vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vLendLook;

		m_eJumpState = STATE_RUN;
	}


	if (pGameInstance->Key_Pressing(DIK_A))
	{
		vTotalLook += -1.f * vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vCamRight;

		m_eJumpState = STATE_RUN;
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		vTotalLook += vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vCamRight;

		m_eJumpState = STATE_RUN;
	}


	if (!pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		if (m_fMageGageTime < m_fMageTimeAcc)
			m_TickStates.push_back(STATE_MAGEDROW);
	}

	vTotalLook = XMVector3Normalize(vTotalLook);
	XMStoreFloat3(&m_vDestLook, vTotalLook);


	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::MageDrow_Input(_float fTimeDelta)
{
}

void CPlayer::Talk_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_E))
	{
		m_TickStates.push_back(STATE_IDLE);
	}

	RELEASE_INSTANCE(CGameInstance);

}




void CPlayer::Anim_Face(_float fTimeDelta)
{
	if (0 != m_FaceAnimIndex[0])
		m_fAnimFaceAcc += fTimeDelta * 200.f;
	else
		m_fAnimFaceAcc += fTimeDelta * 0.5f;

	if (1.f < m_fAnimFaceAcc)
	{
		if (m_bWingk)
			--m_FaceAnimIndex[0];
		else
			++m_FaceAnimIndex[0];

		if (m_FaceAnimIndex[1] - 1 <= m_FaceAnimIndex[0] || 0 >= m_FaceAnimIndex[0])
		{
			if (m_bWingk)
				m_bWingk = false;
			else
				m_bWingk = true;
		}

		m_fAnimFaceAcc = 0.f;
	}
}

void CPlayer::Check_Attacked(_float fTimeDelta)
{
	if (!m_bAttacked)
		return;


	m_fAttackedBoolTimeAcc += fTimeDelta;
	if (3.f < m_fAttackedBoolTimeAcc)
	{
		m_bAttacked = false;
		m_fAttackedBoolTimeAcc = 0.f;
	}

}

#pragma endregion 






#pragma region LateTick


void CPlayer::LateTick(_float fTimeDelta)
{
	if (CToolManager::Get_Instance()->Get_IsLoading())
		return;

	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_PLAYER);
	
	if (nullptr == m_pRendererCom)
		return;

	_bool bisIsWisp = CGameManager::Get_Instance()->Check_IsInWisp(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_bool bisIsWispX = CGameManager::Get_Instance()->Check_IsInWispX(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	// 중력 적용
	if((STATE_STARTGETITEM != m_eState && STATE_IDLEGETITEM != m_eState && STATE_ENDGETITEM != m_eState)
		&& STATE_ATTACKED != m_eState && STATE_JUMPATTACK != m_eState)
		m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 3.f, 0.f, bisIsWispX);

	// 이동 갱신
	Calcul_State(fTimeDelta);

	// 맵에서 떨어졌는지. 세이브 포인트
	OnDipY();

	// 여기서 셀에 있는 콜라이더와 충돌처리 확인하고 밀린다.
	Tick_Col(m_pTransformCom->Get_WorldMatrix(), m_pNavigationCom, m_pTransformCom, 0.f, bisIsWisp);


	// 착지 했다면
	_float fTemp = 0.f;
	if ((m_pNavigationCom->isGround(m_pTransformCom->Get_State(CTransform::STATE_POSITION), &fTemp, 0.f) && !bisIsWispX) || COBB::COL_ON == Get_StaticOBB()->Get_ColState() || COBB::COL_SLIDE == Get_StaticOBB()->Get_ColState())
	{
		if (STATE_JUMP == m_eState || STATE_RUNJUMP == m_eState || STATE_SPRINTJUMP == m_eState || STATE_DOUBLEJUMP == m_eState)
		{
			switch (m_eJumpState)
			{
			case STATE_IDLE:
				m_TickStates.push_back(STATE_JUMPLENDING);
				m_pTransformCom->Set_CurSpeed(0.1f);
				break;
			case STATE_WALK:
				m_TickStates.push_back(STATE_JUMPLENDING);
				m_pTransformCom->Set_CurSpeed(m_fWalkSpeed);
				break;
			case STATE_RUN:
				m_TickStates.push_back(STATE_RUNJUMPLENDING);
				m_pTransformCom->Set_CurSpeed(m_fRunSpeed);
				break;
			case STATE_SPRINT:
				m_TickStates.push_back(STATE_SPRINT);
				m_pTransformCom->Set_CurSpeed(m_fRunSpeed + 2.f);
				break;
			}

		}
		else if (STATE_MAGEJUMP == m_eState)
		{
			switch (m_eJumpState)
			{
			case STATE_IDLE:
				m_TickStates.push_back(STATE_MAGEIDLEJUMPRENDING);
				m_pTransformCom->Set_CurSpeed(0.1f);
				break;
			case STATE_RUN:
				m_TickStates.push_back(STATE_MAGERUNJUMPRENDING);
				m_pTransformCom->Set_CurSpeed(m_fRunSpeed);
				break;
			}
		}
		else if (STATE_SLIDE == m_eState)
			m_TickStates.push_back(STATE_SLIDELENDING);
		else if (STATE_DOWN == m_eState)
			m_pTransformCom->Set_CurSpeed(0.f);

		if (COBB::COL_SLIDE == Get_StaticOBB()->Get_ColState())
		{
			m_TickStates.push_back(STATE_HILLDOWN);
		}

	}


	// 상태 갱신
	Set_State();




	// 애니메이션 END 이벤트
	if (m_pModelCom->Play_Animation(fTimeDelta))
		Check_EndAnim();


	// 소켓 갱신
	if (!m_bRenderSkip)
	{
		m_pSockatCom->Tick(fTimeDelta, m_pTransformCom);
		if(m_bDark)
			m_pSockatCom->LateTick(fTimeDelta, m_pRendererCom, CRenderer::RENDER_NONLIGHT);
		else
			m_pSockatCom->LateTick(fTimeDelta, m_pRendererCom);

		if (STATE_NONE != m_eState)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);


			if(!m_bDark)
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
			else
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
			
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_PLAYER, this);
			RELEASE_INSTANCE(CGameInstance);
		}
	}


	// ============== Clear Tick ===========

	m_pNearMonsters.clear();
}

void CPlayer::Calcul_State(_float fTimeDelta)
{

	if (STATE_IDLE == m_eState || STATE_MAGEIDLE == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(0.f);
	}
	else if (STATE_WALK == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(m_fWalkSpeed);
	}
	else if (STATE_RUN == m_eState || STATE_MAGERUN == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(m_fRunSpeed);
	}
	else if (STATE_SPRINT == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(m_fRunSpeed + 3.f);
	}
	else if (STATE_SLEP == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(m_fSlepSpeed, false);
	}
	else if (STATE_SLIDELENDING == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(m_fSlepSpeed);
	}
	else if (STATE_JUMPLENDING == m_eState || STATE_RUNJUMPLENDING == m_eState)
	{
		if(STATE_IDLE == m_eJumpState)
			m_pTransformCom->Set_CurSpeed(0.01f);
	}
	else if (STATE_JUMP == m_eState || STATE_RUNJUMP == m_eState || STATE_SPRINTJUMP == m_eState || STATE_DOUBLEJUMP == m_eState || STATE_SLIDE == m_eState)
	{	
		// 점프상태로 벽에 부딪혔으면
 		if (COBB::COL_BLOCK == Get_StaticOBB()->Get_ColState())
			m_pTransformCom->Set_CurSpeed(0.f);

	}
	else if (STATE_ATTACK_1 == m_eState || STATE_ATTACK_2 == m_eState || STATE_ATTACK_3 == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(0.f);
	}
	else if (STATE_HILLDOWN == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(0.f);
	}
	else if (STATE_STARTGETITEM == m_eState || STATE_IDLEGETITEM == m_eState || STATE_ENDGETITEM == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(0.f);
	}
	else if (STATE_MAGEDROW == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(0.f);
	}
	else if (STATE_ATTACKED == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(0.f);
	}
	else if (STATE_NONE == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(0.f);
	}
	else if (STATE_APPEAR == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(0.f);
	}

	m_pTransformCom->Go_Straight(m_pTransformCom->Get_CurSpeed() * m_fSlowSpeed, fTimeDelta, m_pNavigationCom);
	m_fSlowSpeed = 1.f;
}

void CPlayer::Check_EndAnim()
{

	// _uint index = m_pModelCom->Get_CurAnimIndex();


	switch (m_eState)
	{
	case STATE_APPEAR:
		m_TickStates.push_back(STATE_IDLE);
		break;
	case STATE_ATTACK_1:
		m_TickStates.push_back(STATE_READYATTACK);
		break;
	case STATE_ATTACK_2:
		m_TickStates.push_back(STATE_READYATTACK);
		break;
	case STATE_ATTACK_3:
		m_TickStates.push_back(STATE_IDLE);
		// m_pSockatCom->Remove_Sockat_If(SLOT_HAND, string("Umbrella"));
		break;
	case STATE_SLEP:
		m_TickStates.push_back(STATE_RUN);
		break;
	case STATE_JUMPLENDING:
		m_TickStates.push_back(STATE_IDLE);
		break;
	case STATE_RUNJUMPLENDING:
		m_TickStates.push_back(STATE_RUN);
		break;	
	case STATE_MAGEIDLEJUMPRENDING:
		m_TickStates.push_back(STATE_MAGEIDLE);
		break;
	case STATE_MAGERUNJUMPRENDING:
		m_TickStates.push_back(STATE_MAGERUN);
		break;
	case STATE_STARTGETITEM:
		m_TickStates.push_back(STATE_IDLEGETITEM);
		break;
	case STATE_ENDGETITEM:
		m_TickStates.push_back(STATE_IDLE);
		CToolManager::Get_Instance()->Set_WithOutPlayer(1.f);
		CCamManager::Get_Instance()->End_CutScene();
		break;
	case STATE_MAGEDROW:
		m_TickStates.push_back(STATE_IDLE);
		break;
	}


}


#pragma	endregion 






#pragma region Render


HRESULT CPlayer::Render()
{
	if (CToolManager::Get_Instance()->Get_IsLoading())
		return S_OK;

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
		Choose_Pass(i);
	}


	if (CToolManager::Get_Instance()->Get_Debug())
	{
		Render_Col();
		
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float fX = XMVectorGetX(vPos);
		_float fY = XMVectorGetY(vPos);
		_float fZ = XMVectorGetZ(vPos);
		wstring str = to_wstring(fX) + TEXT(" / ") + to_wstring(fY) + TEXT(" / ") + to_wstring(fZ);
		CToolManager::Get_Instance()->Render_Fonts(TEXT("Font_Nexon"), str.data(), _float2(50.f, 50.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), _float2(1.f, 1.f), false);
	}



	return S_OK;
}

HRESULT CPlayer::Render_ShadowDepth()
{
	if (CToolManager::Get_Instance()->Get_IsLoading())
		return S_OK;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pTransformCom)
		return E_FAIL;



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;



	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vVPos; XMStoreFloat3(&vVPos, vPos);
	pGameInstance->Set_PlayerPos(vVPos);
	_vector		vLightAt = XMVectorSetW(vPos, 1.f);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 5.f);
	vPos = XMVectorSetX(vPos, XMVectorGetX(vPos) - 7.f);
	_vector		vLightEye = XMVectorSetW(vPos, 1.f);
	_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 1.f);

	_float4x4		LightViewMatrix;
	_matrix temp = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);
	XMStoreFloat4x4(&LightViewMatrix, XMMatrixTranspose(temp));
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &LightViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(0), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 8)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CPlayer::Choose_Pass(_int iIndex)
{
	_uint iPassIndex = 0;

	switch (m_pModelCom->Get_MaterialIndex(iIndex))
	{
		case 3:	// 눈 
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(iIndex), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Set_RawValue("g_iFaceIndex", &m_FaceAnimIndex[0], sizeof(int))))
				return E_FAIL;
			if (FAILED(m_pTextureCom_SmartEye->Set_SRV(m_pShaderCom, "g_SmartEyeTexture", 0)))
				return E_FAIL;


			iPassIndex = 1;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, iIndex, iPassIndex)))
				return E_FAIL;
		}
		break;
		default:
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(iIndex), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
		}
		break;
	}


	if (FAILED(m_pModelCom->Render(m_pShaderCom, iIndex, iPassIndex)))
		return E_FAIL;

}

void CPlayer::Get_Item(CItem::ITEMINVENDESC Desc)
{
	m_TickStates.push_back(STATE_STARTGETITEM);


	char szName[MAX_PATH];
	CToolManager::Get_Instance()->TCtoC(Desc.szModelName, szName);

	Equip_Sockat(szName, SLOT_HAND);

	CCamManager::Get_Instance()->Play_CutScene(1, false, m_pTransformCom);

	CItemManager::Get_Instance()->Add_Item(Desc.szModelName, Desc.iCount);

}

void CPlayer::Get_Hat(TCHAR * szModelName, _bool bAction)
{

	char szName[MAX_PATH];
	CToolManager::Get_Instance()->TCtoC(szModelName, szName);

	if (!bAction)
	{
		Equip_Sockat(szName, SLOT_HAND);
		m_TickStates.push_back(STATE_STARTGETITEM);
		CCamManager::Get_Instance()->Play_CutScene(1, false, m_pTransformCom);
	}

	CItemManager::Get_Instance()->Add_Hat(szModelName);
}

#pragma endregion









#pragma region Init


HRESULT CPlayer::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("HatGirl"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture_SmartEye */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SmartEye"), TEXT("Com_Texture_SmartEye"), (CComponent**)&m_pTextureCom_SmartEye)))
		return E_FAIL;



	if (FAILED(Ready_Sockat()))
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

	//ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	//if (FAILED(AddCollider(CCollider::TYPE_AABB, ColDesc)))
	//	return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(10.f, 10.f, 10.f);
	strcpy(ColDesc.sTag, "Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	strcpy(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	//NaviDesc.iCurrentIndex = 13444;
	//NaviDesc.iCurrentIndex = 5841;
	NaviDesc.iCurrentIndex = 0;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;
	


	CWisp::WISPDESC WispDesc;
	WispDesc.pOwner = this;
	WispDesc.fMaxRatio = 10.f;
	WispDesc.fSpeed = 1.f;

	CGameObject* pObj = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Wisp"), LEVEL_STATIC, TEXT("Layer_Wisp"), &pObj, &WispDesc)))
		return E_FAIL;
	m_pWisp = (CWisp*)pObj;
	Safe_AddRef(m_pWisp);
	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CPlayer::Ready_Sockat()
{
	/* For.Com_Sockat */
	CSockat::SOCATDESC SockatDesc;
	ZeroMemory(&SockatDesc, sizeof(CSockat::SOCATDESC));
	XMStoreFloat4x4(&SockatDesc.mPivot, m_pModelCom->Get_PivotMatrix());
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sockat"), TEXT("Com_Sockat"), (CComponent**)&m_pSockatCom, &SockatDesc)))
		return E_FAIL;


	if (FAILED(m_pSockatCom->Match_Enum_BoneName("bip_hat01", SLOT_HAT)))
		return E_FAIL;
	if (FAILED(m_pSockatCom->Match_Enum_BoneName("bip_ItemPalmR01", SLOT_HAND)))
		return E_FAIL;


	// Equip_Sockat(string("Ori_Hat"), SLOT_HAT);
	// Equip_Sockat(string("Umbrella"), SLOT_HAND);



	return S_OK;
}

HRESULT CPlayer::Equip_Sockat(string sItemName, SLOT eSlot)
{

	// 중복으로 착용하려고 그럼 나간다
	if (m_pSockatCom->Check_IsHaveSocket(eSlot, sItemName))
		return S_OK;

	// Item을 착용 중이냐
	// 그럼 기존거를 삭제하고 끼운다
	if (!m_pSockatCom->Check_Sockat(eSlot))
		m_pSockatCom->Remove_Sockat(eSlot);


	TCHAR cItemName[MAX_PATH];
	char cTempItem[MAX_PATH];
	strcpy(cTempItem, sItemName.data());
	CToolManager::Get_Instance()->CtoTC(cTempItem, cItemName);


	CSockat::PARTSDESC PartsDesc;
	ZeroMemory(&PartsDesc, sizeof(CSockat::PARTSDESC));


	if (!lstrcmp(cItemName, TEXT("Ori_Hat")))
	{
		lstrcpy(PartsDesc.m_szModelName, TEXT("Ori_Hat"));
		PartsDesc.vPos = _float3(0.f, 0.f, -0.63f);
		PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
		PartsDesc.vRot = _float3(-90.f, 0, 0);
		PartsDesc.pOwner = this;


		_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.0f, 0.7f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.8f, 0.8f, 0.8f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.1f, 1.5f, false, 0.f, 0.f, 0.75f,
			7, 0.f, 0.1f, 0.2f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_MODLE);
		CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_Star"), vPos, _float3(0.f, 0.5f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.1f, 3.f, true, 1.f, 1.f, 1.f,
			5, 0.f, 0.2f, 0.f, 0.f, 0.f, 0.2f, 0.f, 0.1f, 1.f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_TEXTURE);

	}
	else if (!lstrcmp(cItemName, TEXT("Sprint_Hat")))
	{
		if (SLOT_HAND == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Sprint_Hat"));
			PartsDesc.vPos = _float3(-0.33f, -0.2f, -0.4f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-180.f, 0.f, 0.f);
			PartsDesc.pOwner = this;
		}
		else if(SLOT_HAT == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Sprint_Hat"));
			PartsDesc.vPos = _float3(0.f, -0.03f, -0.72f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-82.9f, 0.f, 0.f);
			PartsDesc.pOwner = this;

			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.0f, 0.7f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.8f, 0.8f, 0.8f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.1f, 1.5f, false, 0.f, 0.f, 0.75f,
				7, 0.f, 0.1f, 0.2f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_MODLE);
			CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_Star"), vPos, _float3(0.f, 0.5f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.1f, 3.f, true, 1.f, 1.f, 1.f,
				5, 0.f, 0.2f, 0.f, 0.f, 0.f, 0.2f, 0.f, 0.1f, 1.f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_TEXTURE);
		}

	}
	else if (!lstrcmp(cItemName, TEXT("Witch_Hat")))
	{
		if (SLOT_HAND == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Witch_Hat"));
			PartsDesc.vPos = _float3(-0.33f, -0.2f, -0.4f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-180.f, 0.f, 0.f);
			PartsDesc.pOwner = this;
		}
		else if(SLOT_HAT == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Witch_Hat")); 
			PartsDesc.vPos = _float3(0.f, -0.01f, -0.81f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-90.f, 0.f, -0.f);
			PartsDesc.pOwner = this;

			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.0f, 0.7f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.8f, 0.8f, 0.8f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.1f, 1.5f, false, 0.f, 0.f, 0.75f,
				7, 0.f, 0.1f, 0.2f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_MODLE);
			CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_Star"), vPos, _float3(0.f, 0.5f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.1f, 3.f, true, 1.f, 1.f, 1.f,
				5, 0.f, 0.2f, 0.f, 0.f, 0.f, 0.2f, 0.f, 0.1f, 1.f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_TEXTURE);
		}
	}
	else if (!lstrcmp(cItemName, TEXT("Mask_Cat")))
	{
		if (SLOT_HAND == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Mask_Cat"));
			PartsDesc.vPos = _float3(-0.33f, -0.2f, -0.4f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-180.f, 0.f, 0.f);
			PartsDesc.pOwner = this;
		}
		else if (SLOT_HAT == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Mask_Cat"));
			PartsDesc.vPos = _float3(0.f, -0.14f, -0.58f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-90.f, 0.f, -0.f);
			PartsDesc.pOwner = this;

			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.0f, 0.7f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.8f, 0.8f, 0.8f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.1f, 1.5f, false, 0.f, 0.f, 0.75f,
				7, 0.f, 0.1f, 0.2f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_MODLE);
			CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_Star"), vPos, _float3(0.f, 0.5f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.1f, 3.f, true, 1.f, 1.f, 1.f,
				5, 0.f, 0.2f, 0.f, 0.f, 0.f, 0.2f, 0.f, 0.1f, 1.f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_TEXTURE);
		}
	}
	else if (!lstrcmp(cItemName, TEXT("Mask_Fox")))
	{
		if (SLOT_HAND == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Mask_Fox"));
			PartsDesc.vPos = _float3(-0.33f, -0.2f, -0.4f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-180.f, 0.f, 0.f);
			PartsDesc.pOwner = this;
		}
		else if (SLOT_HAT == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Mask_Fox"));
			PartsDesc.vPos = _float3(0.f, -0.14f, -0.58f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-90.f, 0.f, -0.f);
			PartsDesc.pOwner = this;

			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos, _float3(0.0f, 0.7f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.8f, 0.8f, 0.8f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.1f, 1.5f, false, 0.f, 0.f, 0.75f,
				7, 0.f, 0.1f, 0.2f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_MODLE);
			CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_Star"), vPos, _float3(0.f, 0.5f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.1f, 3.f, true, 1.f, 1.f, 1.f,
				5, 0.f, 0.2f, 0.f, 0.f, 0.f, 0.2f, 0.f, 0.1f, 1.f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_TEXTURE);
		}
	}
	else if (!lstrcmp(cItemName, TEXT("Umbrella")))
	{
		lstrcpy(PartsDesc.m_szModelName, TEXT("Umbrella"));
		PartsDesc.vPos = _float3(-0.28f, 0.11f, -0.41f);
		PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
		PartsDesc.vRot = _float3(0.f, 0.f, 178.f);
		PartsDesc.pOwner = this;

		if (!FAILED(m_pSockatCom->Add_Sockat(eSlot, m_pModelCom, TEXT("Prototype_GameObject_Umbrella"), PartsDesc)))
			return E_FAIL;

		return S_OK;
	}
	else if (!lstrcmp(cItemName, TEXT("yarn_ui_brew")))
	{
		lstrcpy(PartsDesc.m_szModelName, TEXT("yarn_ui_brew"));
		PartsDesc.vPos = _float3(-0.25, -0.11f, -0.44f);
		PartsDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
		PartsDesc.vRot = _float3(0.f, 0.f, -158.2f);
		PartsDesc.pOwner = this;
	}
	else if (!lstrcmp(cItemName, TEXT("yarn_ui_hover")))
	{
		lstrcpy(PartsDesc.m_szModelName, TEXT("yarn_ui_hover"));
		PartsDesc.vPos = _float3(-0.25, -0.11f, -0.44f);
		PartsDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
		PartsDesc.vRot = _float3(0.f, 0.f, -158.2f);
		PartsDesc.pOwner = this;
	}
	else if (!lstrcmp(cItemName, TEXT("yarn_ui_ice")))
	{
		lstrcpy(PartsDesc.m_szModelName, TEXT("yarn_ui_ice"));
		PartsDesc.vPos = _float3(-0.25, -0.11f, -0.44f);
		PartsDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
		PartsDesc.vRot = _float3(0.f, 0.f, -158.2f);
		PartsDesc.pOwner = this;
	}
	else if (!lstrcmp(cItemName, TEXT("yarn_ui_sprint")))
	{
		lstrcpy(PartsDesc.m_szModelName, TEXT("yarn_ui_sprint"));
		PartsDesc.vPos = _float3(-0.25, -0.11f, -0.44f);
		PartsDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
		PartsDesc.vRot = _float3(0.f, 0.f, -158.2f);
		PartsDesc.pOwner = this;
	}
	else if (!lstrcmp(cItemName, TEXT("science_owlbrew_remade")))
	{
		lstrcpy(PartsDesc.m_szModelName, TEXT("science_owlbrew_remade"));
		PartsDesc.vPos = _float3(-0.28, 0.03f, -0.41f);
		PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
		PartsDesc.vRot = _float3(180.f, 0.f, 0.f);
		PartsDesc.pOwner = this;

		CUIManager::Get_Instance()->Make_WitchChargEffect();
	}
	else
		return E_FAIL;


	if (!FAILED(m_pSockatCom->Add_Sockat(eSlot, m_pModelCom, TEXT("Prototype_GameObject_Parts"), PartsDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Attacked()
{
	m_TickStates.push_back(STATE_ATTACKED);
	m_bAttacked = true;
}

void CPlayer::Find_NearstMonster()
{

	// 제일 가까운 몬스터를 찾는다.
	m_pNearstMonster = nullptr;
	_float fMinDis = FLT_MAX;
	for (auto& pMonster : m_pNearMonsters)
	{
		CTransform* pMonsterTran = (CTransform*)pMonster->Get_ComponentPtr(TEXT("Com_Transform"));
		_vector vMonsterPos = pMonsterTran->Get_State(CTransform::STATE_POSITION);
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float fDis = XMVectorGetX(XMVector3Length(vMonsterPos - vMyPos));

		if (fMinDis > fDis)
		{
			m_pNearstMonster = pMonster;
			fMinDis = fDis;
		}
	}

}

void CPlayer::SetPosNavi(LEVEL eLevel, _fvector vPos)
{
	Safe_Release(m_pNavigationCom);

	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;
	if (FAILED(__super::Add_Component(eLevel, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation1"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return;

	_vector vVPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vVPos);
	_uint iIndex =  CToolManager::Get_Instance()->Find_NaviIndex(vVPos);
	m_pNavigationCom->Set_NaviIndex(iIndex);
}





#pragma endregion






void CPlayer::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if(("Tag_PuzzleCube_Boss" == Desc.pOther->Get_Tag() || "Tag_PuzzleCube" == Desc.pOther->Get_Tag() || "Tag_Barrel" == Desc.pOther->Get_Tag() || "Tag_IceBox" == Desc.pOther->Get_Tag()) && !strcmp("Attacked_Sphere", Desc.MyDesc.sTag))
		Get_StaticOBB()->Compute_Pigi(Desc.pOther, m_pNavigationCom, m_pTransformCom, CGameManager::Get_Instance()->Check_IsInWisp(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	
	if ("Tag_Monster" == Desc.pOther->Get_Tag() || "Tag_Snatcher" == Desc.pOther->Get_Tag())
	{
		if(!strcmp("Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
			m_pNearMonsters.push_back(Desc.pOther);

		if (!strcmp("Attacked_Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
		{
			if (STATE_JUMPATTACK == m_eState && Desc.pOther == Get_NearstMonster())
			{
				// 점프 공격
				if ("Tag_Snatcher" == Desc.pOther->Get_Tag())
				{
					((CVSnatcher*)Desc.pOther)->Attacked();
					m_TickStates.push_back(STATE_DOUBLEJUMP);
					m_pTransformCom->DoubleJump(m_fJumpPower + 4.f);

					_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
					CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_Star"), vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.3f, 0.3f, 0.3f), _float3(0.3f, 0.3f, 0.3f), _float3(0.f, 0.f, 0.f), _float3(-90.f, 0.f, 0.f), 0.1f, 3.f, false, 0.f, 0.f, 2.f,
						8, 1.f, 0.1f, 0.f, 0.f, 0.f, 0.2f, 0.f, 0.0f, 0.2f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_TEXTURE);
				}
				else
				{
					((CMonster*)Desc.pOther)->Attacked(1);
					m_TickStates.push_back(STATE_DOUBLEJUMP);
					m_pTransformCom->DoubleJump(m_fJumpPower + 4.f);

					_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
					CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_Star"), vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.3f, 0.3f, 0.3f), _float3(0.3f, 0.3f, 0.3f), _float3(0.f, 0.f, 0.f), _float3(-90.f, 0.f, 0.f), 0.1f, 3.f, false, 0.f, 0.f, 2.f,
						8, 1.f, 0.1f, 0.f, 0.f, 0.f, 0.2f, 0.f, 0.0f, 0.2f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_TEXTURE);
				}

			}

		}
			
	}


}

void CPlayer::OnDipY()
{
	_float fY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (-20.f > fY)
	{
		// 가장 최근에 친 종으로 이동한다.
		_uint iNaviIndex = 0;
		_float3 vNaviPos; ZeroMemory(&vNaviPos, sizeof(_float3));
		CGameManager::Get_Instance()->Get_NaviPoint(&iNaviIndex, &vNaviPos);

		m_pNavigationCom->Set_NaviIndex(iNaviIndex);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vNaviPos), 1.f));

		m_pTransformCom->ResetGravity();

		Attacked();
	}

}





CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pWisp);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTextureCom_SmartEye);
	Safe_Release(m_pSockatCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
