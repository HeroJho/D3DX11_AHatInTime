#include "stdafx.h"
#include "..\Public\AnimPlayer.h"
#include "GameInstance.h"
#include "Parts.h"

#include "AnimManager.h"
#include "Camera_Free.h"

CAnimPlayer::CAnimPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CAnimPlayer::CAnimPlayer(const CAnimPlayer & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
	ZeroMemory(&m_vDestLook, sizeof(_float3));
}

HRESULT CAnimPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimPlayer::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eState = STATE_IDLE;

	m_fWalkSpeed = 1.f;
	m_fRunSpeed = 2.5f;
	m_fTurnSpeed = 1.f;
	m_fRotationSpeed = 3.5f;

	m_fCulSpeed = m_fWalkSpeed;

	m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.5f, 0.f, 0.5f, 1.f));

	return S_OK;
}





#pragma region GET_SET

_float CAnimPlayer::Get_AnimSpeed(STATE eState)
{
	switch (eState)
	{
	case Client::CAnimPlayer::STATE_IDLE:
		return m_pModelCom->Get_Anim_TickPerSecond(113);
	case Client::CAnimPlayer::STATE_WALK:
		return m_pModelCom->Get_Anim_TickPerSecond(192);
	case Client::CAnimPlayer::STATE_RUN:
		return m_pModelCom->Get_Anim_TickPerSecond(193);
	case Client::CAnimPlayer::STATE_SLEP:
		return m_pModelCom->Get_Anim_TickPerSecond(180);
	case Client::CAnimPlayer::STATE_JUMPLENDING:
		return m_pModelCom->Get_Anim_TickPerSecond(103);
	case Client::CAnimPlayer::STATE_RUNJUMPLENDING:
		return m_pModelCom->Get_Anim_TickPerSecond(104);
	case Client::CAnimPlayer::STATE_SPRINTJUMP:
		return m_pModelCom->Get_Anim_TickPerSecond(106);
	case Client::CAnimPlayer::STATE_DOUBLEJUMP:
		return m_pModelCom->Get_Anim_TickPerSecond(92);
	case Client::CAnimPlayer::STATE_SLIDE:
		return m_pModelCom->Get_Anim_TickPerSecond(97);
	case Client::CAnimPlayer::STATE_SLIDELENDING:
		return m_pModelCom->Get_Anim_TickPerSecond(101);
	}

}

void CAnimPlayer::Set_AnimSpeed(STATE eState, _float fSpeed)
{
	switch (eState)
	{
	case Client::CAnimPlayer::STATE_IDLE:
		m_pModelCom->Set_Anim_TickPerSecond(111, fSpeed);
		break;
	case Client::CAnimPlayer::STATE_WALK:
		m_pModelCom->Set_Anim_TickPerSecond(197, fSpeed);
		break;
	case Client::CAnimPlayer::STATE_RUN:
		m_pModelCom->Set_Anim_TickPerSecond(198, fSpeed);
		break;
	case Client::CAnimPlayer::STATE_SLEP:
		m_pModelCom->Set_Anim_TickPerSecond(177, fSpeed);
		break;
	case Client::CAnimPlayer::STATE_JUMPLENDING:
		m_pModelCom->Set_Anim_TickPerSecond(103, fSpeed);
		break;
	case Client::CAnimPlayer::STATE_RUNJUMPLENDING:
		m_pModelCom->Set_Anim_TickPerSecond(104, fSpeed);
		break;
	case Client::CAnimPlayer::STATE_SPRINTJUMP:
		m_pModelCom->Set_Anim_TickPerSecond(106, fSpeed);
		break;
	case Client::CAnimPlayer::STATE_DOUBLEJUMP:
		m_pModelCom->Set_Anim_TickPerSecond(92, fSpeed);
		break;
	case Client::CAnimPlayer::STATE_SLIDE:
		m_pModelCom->Set_Anim_TickPerSecond(97, fSpeed);
		break;
	case Client::CAnimPlayer::STATE_SLIDELENDING:
		m_pModelCom->Set_Anim_TickPerSecond(101, fSpeed);
		break;
	}
}

void CAnimPlayer::Set_AnimLinearData(ANIM_LINEAR_DATA LinearData)
{
	m_pModelCom->Push_AnimLinearData(LinearData);
}

void CAnimPlayer::Reset_AnimLinearData()
{
	m_pModelCom->Reset_AnimLinearData();
}


void CAnimPlayer::Set_State()
{
	if (m_TickStates.empty())
		return;

	list<STATE>::iterator iter = max_element(m_TickStates.begin(), m_TickStates.end());


	m_ePreState = m_eState;
	m_eState = (*iter);





	switch (m_eState)
	{
	case STATE_SLEP:
		m_fSlepSpeed = 2.f;
		break;
	case STATE_SLIDELENDING:
		m_fSlepSpeed = m_fCulSpeed;
		break;
	default:
		m_fSlepSpeed = 0.f;
		break;
	}





	// 애니 갱신
	Set_Anim();

	m_TickStates.clear();
}

void CAnimPlayer::Set_Anim()
{
	switch (m_eState)
	{
	case STATE_IDLE:
		m_pModelCom->Set_AnimIndex(113);
		break;
	case STATE_WALK:
		m_pModelCom->Set_AnimIndex(192);
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
		break;
	case STATE_SPRINT:
		m_pModelCom->Set_AnimIndex(153);
		break;
	case STATE_JUMP:
		m_pModelCom->Set_AnimIndex(96);
		break;
	case STATE_JUMPLENDING:
		m_pModelCom->Set_AnimIndex(103);
		break;
	case STATE_RUNJUMP:
		m_pModelCom->Set_AnimIndex(94);
		break;
	case STATE_RUNJUMPLENDING:
		m_pModelCom->Set_AnimIndex(104);
		break;
	case STATE_SPRINTJUMP:
		m_pModelCom->Set_AnimIndex(106);
		break;
	case STATE_DOUBLEJUMP:
		m_pModelCom->Set_AnimIndex(92);
		break;
	case STATE_SLIDE:
		m_pModelCom->Set_AnimIndex(97);
		break;
	case STATE_SLIDELENDING:
		m_pModelCom->Set_AnimIndex(101);
		break;
	case STATE_ATTACK_1:
		m_pModelCom->Set_AnimIndex(188);
		break;
	case STATE_ATTACK_2:
		m_pModelCom->Set_AnimIndex(189);
		break;
	case STATE_ATTACK_3:
		m_pModelCom->Set_AnimIndex(190);
		break;
	case STATE_SLEP:
		m_pModelCom->Set_AnimIndex(180);
		break;
	case STATE_STATU:
		m_pModelCom->Set_AnimIndex(204);
		break;
	}
}

#pragma endregion





#pragma region Tick

void CAnimPlayer::Tick(_float fTimeDelta)
{


	if (m_bStatu)
		m_TickStates.push_back(STATE_STATU);

	if (CAnimManager::Get_Instance()->Get_GameMode())
		Game_Mode(fTimeDelta);
	else
		Tool_Mode(fTimeDelta);



	m_pSockatCom->Tick(fTimeDelta, m_pTransformCom);
}

void CAnimPlayer::Tool_Mode(_float fTimeDelta)
{
	m_TickStates.push_back(STATE_IDLE);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (pGameInstance->Key_Pressing(DIK_W))
	{

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_pTransformCom->Go_Straight(m_fRunSpeed, fTimeDelta);
			m_TickStates.push_back(STATE_RUN);
		}
		else
		{
			m_pTransformCom->Go_Straight(m_fWalkSpeed, fTimeDelta);
			m_TickStates.push_back(STATE_WALK);
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{

	}

	if (pGameInstance->Key_Pressing(DIK_A))
	{
		if (!pGameInstance->Key_Pressing(DIK_W))
			m_pTransformCom->Go_Straight(m_fTurnSpeed, fTimeDelta);

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -m_fRotationSpeed, fTimeDelta);
		m_TickStates.push_back(STATE_WALK);
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		if (!pGameInstance->Key_Pressing(DIK_W))
			m_pTransformCom->Go_Straight(m_fTurnSpeed, fTimeDelta);

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRotationSpeed, fTimeDelta);
		m_TickStates.push_back(STATE_WALK);
	}
	


	RELEASE_INSTANCE(CGameInstance);
}


void CAnimPlayer::Game_Mode(_float fTimeDelta)
{
	Anim_Face(fTimeDelta);

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
	case STATE_JUMPLENDING :
	case STATE_RUNJUMPLENDING:
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
	}

}



void CAnimPlayer::Idle_Tick(_float fTimeDelta)
{
	if (STATE_SPRINT == m_ePreState)
	{
		m_bImStop = true;
		m_TickStates.push_back(STATE_SLEP);
	}
}

void CAnimPlayer::Move_Tick(_float fTimeDelta)
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

void CAnimPlayer::Jump_Tick(_float fTimeDelta)
{
	m_eJumpState = STATE_IDLE;

	Jump_Input(fTimeDelta);

	m_pTransformCom->LinearTurn(m_vDestLook, 0.5f, 0.3f, fTimeDelta, false);
}

void CAnimPlayer::DoubleJump_Tick(_float fTimeDelta)
{
	m_eJumpState = STATE_IDLE;

	DoubleJump_Input(fTimeDelta);

	m_pTransformCom->LinearTurn(m_vDestLook, 0.5f, 0.3f, fTimeDelta, false);
}

void CAnimPlayer::SprintJump_Tick(_float fTimeDelta)
{
	m_eJumpState = STATE_IDLE;

	SprintJump_Input(fTimeDelta);

	m_pTransformCom->LinearTurn(m_vDestLook, 0.5f, 0.3f, fTimeDelta, false);
}

void CAnimPlayer::Rend_Tick(_float fTimeDelta)
{
	// m_eJumpState = STATE_IDLE;

	Rend_Input(fTimeDelta);

	if (m_pTransformCom->LinearTurn(m_vDestLook, 0.5f, 0.3f, fTimeDelta))
	{
		m_fSlepSpeed = 2.2f;
		m_TickStates.push_back(STATE_SLEP);
	}
}

void CAnimPlayer::Slep_Tick(_float fTimeDelta)
{
	m_fSlepSpeed -= 6.f * fTimeDelta;
}

void CAnimPlayer::Slide_Tick(_float fTimeDelta)
{
	m_eJumpState = STATE_IDLE;

	Slide_Input(fTimeDelta);

	m_pTransformCom->LinearTurn(m_vDestLook, 0.1f, 0.3f, fTimeDelta, false);
}

void CAnimPlayer::SlideRending_Tick(_float fTimeDelta)
{
	if (0.f < m_fCulSpeed)
		m_fSlepSpeed -= 3.f * fTimeDelta;
	else
		m_fSlepSpeed = 0.f;

	SlideRending_Input(fTimeDelta);
}



void CAnimPlayer::Move_Input(_float fTimeDelta)
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
		// XMStoreFloat3(&m_vDestLook, vLendLook);
		vTotalLook += vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_TickStates.push_back(STATE_RUN);
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_TickStates.push_back(STATE_SPRINT);
		}
		else
		{
			m_TickStates.push_back(STATE_WALK);
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		vTotalLook += -1.f * vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_TickStates.push_back(STATE_RUN);
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_TickStates.push_back(STATE_SPRINT);
		}
		else
		{
			m_TickStates.push_back(STATE_WALK);
		}
	}


	if (pGameInstance->Key_Pressing(DIK_A))
	{
		vTotalLook += -1.f * vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_TickStates.push_back(STATE_RUN);
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_TickStates.push_back(STATE_SPRINT);
		}
		else
		{
			m_TickStates.push_back(STATE_WALK);
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		vTotalLook += vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_TickStates.push_back(STATE_RUN);
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_TickStates.push_back(STATE_SPRINT);
		}
		else
		{
			m_TickStates.push_back(STATE_WALK);
		}
	}


	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pTransformCom->Jump(m_fJumpPower);

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_TickStates.push_back(STATE_RUNJUMP);
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_TickStates.push_back(STATE_SPRINTJUMP);
		}
		else
		{
			m_TickStates.push_back(STATE_JUMP);
		}
	}


	if (pGameInstance->Mouse_Down(DIMK_LBUTTON))
	{
		m_TickStates.push_back(STATE_ATTACK_1);
	}




	vTotalLook = XMVector3Normalize(vTotalLook);
	XMStoreFloat3(&m_vDestLook, vTotalLook);


	RELEASE_INSTANCE(CGameInstance);
}

void CAnimPlayer::Attack_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (pGameInstance->Mouse_Down(DIMK_LBUTTON))
	{
		if (STATE_ATTACK_1 == m_eState)
			m_ComboStates.push_back(STATE_ATTACK_2);
		else if(STATE_ATTACK_2 == m_eState)
			m_ComboStates.push_back(STATE_ATTACK_3);
	}


	RELEASE_INSTANCE(CGameInstance);
}

void CAnimPlayer::ReadyAttack_Input(_float fTimeDelta)
{
	if (m_ComboStates.empty())
	{
		m_TickStates.push_back(STATE_IDLE);
		return;
	}
		
	if (STATE_ATTACK_2 == m_ComboStates.front())
		m_TickStates.push_back(STATE_ATTACK_2);
	else if(STATE_ATTACK_3 == m_ComboStates.front())
		m_TickStates.push_back(STATE_ATTACK_3);
	else
		m_TickStates.push_back(STATE_IDLE);

	m_ComboStates.clear();
}

void CAnimPlayer::Jump_Input(_float fTimeDelta)
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
		// XMStoreFloat3(&m_vDestLook, vLendLook);
		vTotalLook += vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_eJumpState = STATE_RUN;
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_eJumpState = STATE_SPRINT;
		}
		else
		{
			m_eJumpState = STATE_WALK;
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		// XMStoreFloat3(&m_vDestLook, -1.f * vLendLook);
		vTotalLook += -1.f * vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_eJumpState = STATE_RUN;
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_eJumpState = STATE_SPRINT;
		}
		else
		{
			m_eJumpState = STATE_WALK;
		}
	}


	if (pGameInstance->Key_Pressing(DIK_A))
	{
		// XMStoreFloat3(&m_vDestLook, -1.f*vCamRight);
		vTotalLook += -1.f * vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_eJumpState = STATE_RUN;
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_eJumpState = STATE_SPRINT;
		}
		else
		{
			m_eJumpState = STATE_WALK;
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		// XMStoreFloat3(&m_vDestLook, vCamRight);
		vTotalLook += vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_eJumpState = STATE_RUN;
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_eJumpState = STATE_SPRINT;
		}
		else
		{
			m_eJumpState = STATE_WALK;
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

void CAnimPlayer::DoubleJump_Input(_float fTimeDelta)
{
	SprintJump_Input(fTimeDelta);
}

void CAnimPlayer::SprintJump_Input(_float fTimeDelta)
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
		// XMStoreFloat3(&m_vDestLook, vLendLook);
		vTotalLook += vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_eJumpState = STATE_RUN;
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_eJumpState = STATE_SPRINT;
		}
		else
		{
			m_eJumpState = STATE_WALK;
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		// XMStoreFloat3(&m_vDestLook, -1.f * vLendLook);
		vTotalLook += -1.f * vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_eJumpState = STATE_RUN;
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_eJumpState = STATE_SPRINT;
		}
		else
		{
			m_eJumpState = STATE_WALK;
		}
	}


	if (pGameInstance->Key_Pressing(DIK_A))
	{
		// XMStoreFloat3(&m_vDestLook, -1.f*vCamRight);
		vTotalLook += -1.f * vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_eJumpState = STATE_RUN;
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_eJumpState = STATE_SPRINT;
		}
		else
		{
			m_eJumpState = STATE_WALK;
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		// XMStoreFloat3(&m_vDestLook, vCamRight);
		vTotalLook += vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_eJumpState = STATE_RUN;
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_eJumpState = STATE_SPRINT;
		}
		else
		{
			m_eJumpState = STATE_WALK;
		}
	}


	vTotalLook = XMVector3Normalize(vTotalLook);
	XMStoreFloat3(&m_vDestLook, vTotalLook);


	RELEASE_INSTANCE(CGameInstance);
}

void CAnimPlayer::Rend_Input(_float fTimeDleta)
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
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_TickStates.push_back(STATE_SPRINTJUMP);
		}
		else
		{
			m_TickStates.push_back(STATE_JUMP);
		}
	}


	vTotalLook = XMVector3Normalize(vTotalLook);
	XMStoreFloat3(&m_vDestLook, vTotalLook);


	RELEASE_INSTANCE(CGameInstance);
}

void CAnimPlayer::Slide_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(DIK_W))
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_eJumpState = STATE_RUN;
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_eJumpState = STATE_SPRINT;
		}
		else
		{
			m_eJumpState = STATE_WALK;
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_eJumpState = STATE_RUN;
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_eJumpState = STATE_SPRINT;
		}
		else
		{
			m_eJumpState = STATE_WALK;
		}
	}


	if (pGameInstance->Key_Pressing(DIK_A))
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_eJumpState = STATE_RUN;
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_eJumpState = STATE_SPRINT;
		}
		else
		{
			m_eJumpState = STATE_WALK;
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_eJumpState = STATE_RUN;
		}
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
		{
			m_eJumpState = STATE_SPRINT;
		}
		else
		{
			m_eJumpState = STATE_WALK;
		}
	}


	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		m_TickStates.push_back(STATE_DOUBLEJUMP);
		m_pTransformCom->DoubleJump(m_fJumpPower);
	}


	RELEASE_INSTANCE(CGameInstance);
}

void CAnimPlayer::SlideRending_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		m_TickStates.push_back(STATE_DOUBLEJUMP);
		m_pTransformCom->DoubleJump(m_fJumpPower - 2.f);
	}

	RELEASE_INSTANCE(CGameInstance);
}




void CAnimPlayer::Anim_Face(_float fTimeDelta)
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


#pragma endregion 






#pragma region LateTick

void CAnimPlayer::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 0.008f);
	_float fTemp = 0.f;
	if (m_pNavigationCom->isGround(m_pTransformCom->Get_State(CTransform::STATE_POSITION), &fTemp))
	{
		if (STATE_JUMP == m_eState || STATE_RUNJUMP == m_eState || STATE_SPRINTJUMP == m_eState || STATE_DOUBLEJUMP == m_eState)
		{
			switch (m_eJumpState)
			{
			case STATE_IDLE:
				m_TickStates.push_back(STATE_JUMPLENDING);
				break;
			case STATE_WALK:
				m_TickStates.push_back(STATE_JUMPLENDING);
				break;
			case STATE_RUN:
				m_TickStates.push_back(STATE_RUNJUMPLENDING);
				break;
			case STATE_SPRINT:
				m_TickStates.push_back(STATE_SPRINT);
				break;
			}

		}
		else if (STATE_SLIDE == m_eState)
		{
			m_TickStates.push_back(STATE_SLIDELENDING);
		}

	}




	// NOTE : Tick으로 올리면 Trun할 때 문제 생김
	// 상태 갱신
	Set_State();
	// 이동 갱신
	Calcul_State(fTimeDelta);


	// 애니메이션 END 이벤트
	if (m_pModelCom->Play_Animation(fTimeDelta))
		Check_EndAnim();


	m_pSockatCom->Tick(fTimeDelta, m_pTransformCom);


	m_pSockatCom->LateTick(fTimeDelta, m_pRendererCom);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

void CAnimPlayer::Calcul_State(_float fTimeDelta)
{
	if (STATE_WALK == m_eState)
	{
		m_fCulSpeed = m_fWalkSpeed;
	}
	else if (STATE_RUN == m_eState)
	{
		m_fCulSpeed = m_fRunSpeed;
	}
	else if (STATE_SPRINT == m_eState)
	{
		m_fCulSpeed = m_fRunSpeed + 2.f;
	}
	else if (STATE_SLEP == m_eState)
	{
		m_fCulSpeed = m_fSlepSpeed;
	}
	else if (STATE_SLIDELENDING == m_eState)
	{
		m_fCulSpeed = m_fSlepSpeed;
	}
	else if (STATE_JUMP == m_eState || STATE_JUMPLENDING == m_eState || STATE_RUNJUMP == m_eState || STATE_RUNJUMPLENDING == m_eState || STATE_SPRINTJUMP == m_eState || STATE_DOUBLEJUMP == m_eState || STATE_SLIDE == m_eState)
	{

		if (STATE_IDLE != m_eJumpState)
		{
			if (STATE_WALK == m_eJumpState)
				m_fCulSpeed = m_fWalkSpeed;
			else if (STATE_RUN == m_eJumpState)
				m_fCulSpeed = m_fRunSpeed;
			else if (STATE_SPRINT == m_eJumpState)
				m_fCulSpeed = m_fRunSpeed + 2.f;

			if (STATE_SLIDE == m_eState)
				m_fCulSpeed += 4.f;
		}
	}
	else if(STATE_IDLE == m_eState)
	{
		m_fCulSpeed = 0.f;
	}

	m_pTransformCom->Go_Straight(m_fCulSpeed, fTimeDelta, m_pNavigationCom);
}

void CAnimPlayer::Check_EndAnim()
{

	// _uint index = m_pModelCom->Get_CurAnimIndex();


	switch (m_eState)
	{
	case STATE_ATTACK_1:
		m_TickStates.push_back(STATE_READYATTACK);
		break;
	case STATE_ATTACK_2:
		m_TickStates.push_back(STATE_READYATTACK);
		break;
	case STATE_ATTACK_3:
		m_TickStates.push_back(STATE_IDLE);
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
	}


}

#pragma	endregion 






#pragma region Render

HRESULT CAnimPlayer::Render()
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
		Choose_Pass(i);
	}

	return S_OK;
}

HRESULT CAnimPlayer::Choose_Pass(_int iIndex)
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
		//if (FAILED(m_pTextureCom_SmartEye->Set_SRV(m_pShaderCom, "g_SmartEyeTexture", 0)))
		//	return E_FAIL;


		iPassIndex = 1;
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


#pragma endregion









#pragma region Init

HRESULT CAnimPlayer::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, TEXT("HatGirl"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	/* For.Com_Sockat */
	CSockat::SOCATDESC SockatDesc;
	ZeroMemory(&SockatDesc, sizeof(CSockat::SOCATDESC));
	XMStoreFloat4x4(&SockatDesc.mPivot, m_pModelCom->Get_PivotMatrix());
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sockat"), TEXT("Com_Sockat"), (CComponent**)&m_pSockatCom, &SockatDesc)))
		return E_FAIL;



	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;



	return S_OK;
}

CGameObject* CAnimPlayer::Add_Sockat(char* pBoneName, _tchar* cName)
{
	if (nullptr == m_pSockatCom || nullptr == m_pModelCom)
		return nullptr;

	CHierarchyNode*		pSocket = m_pModelCom->Get_HierarchyNode(pBoneName);
	if (nullptr == pSocket)
		return nullptr;


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	CParts::PARTSMODELDESC Desc;
	ZeroMemory(&Desc, sizeof(CParts::PARTSMODELDESC));
	memcpy(Desc.cModelTag, cName, sizeof(_tchar)*MAX_PATH);

	/* For.Sword */
	CGameObject*		pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Parts"), &Desc);
	if (nullptr == pGameObject)
	{
		RELEASE_INSTANCE(CGameInstance);
		return nullptr;
	}



	RELEASE_INSTANCE(CGameInstance);


	m_pSockatCom->Add_Child(pGameObject, pSocket);

	return pGameObject;
}

#pragma endregion




CAnimPlayer * CAnimPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAnimPlayer*		pInstance = new CAnimPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAnimPlayer::Clone(void * pArg)
{
	CAnimPlayer*		pInstance = new CAnimPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAnimPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimPlayer::Free()
{
	__super::Free();


	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pSockatCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
