#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"

#include "DataManager.h"

#include "Camera_Free.h"

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
	if (FAILED(Ready_Components()))
		return E_FAIL;

	list<ANIM_LINEAR_DATA> LinearDatas = CDataManager::Get_Instance()->Load_Anim("HatGirl");

	for (auto& Data : LinearDatas)
	{
		Set_AnimLinearData(Data);
	}


	m_eState = STATE_IDLE;

	m_fWalkSpeed = 1.f;
	m_fRunSpeed = 2.5f;
	m_fTurnSpeed = 1.f;
	m_fRotationSpeed = 3.5f;
	m_vDestLook = _float3{ 0.f, 0.f, 1.f };
	m_pTransformCom->Set_Look(XMLoadFloat3(&m_vDestLook));
	m_pTransformCom->Set_DestLook();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(86.94f, 5.f, 3.87f, 1.f));

	m_pTransformCom->Set_CurSpeed(m_fWalkSpeed);

	m_FaceAnimIndex[0] = 0;
	m_FaceAnimIndex[1] = 32;

	return S_OK;
}





#pragma region GET_SET


void CPlayer::Set_AnimLinearData(ANIM_LINEAR_DATA LinearData)
{
	m_pModelCom->Push_AnimLinearData(LinearData);
}

void CPlayer::Set_State()
{
	if (m_TickStates.empty())
		return;


	list<STATE>::iterator iter = max_element(m_TickStates.begin(), m_TickStates.end());

	
	m_ePreState = m_eState;
	m_eState = (*iter);



	// 애니메이션이 바뀔때 거쳐야하는 부분p
	switch (m_eState)
	{
	case STATE_SLEP:
		m_fSlepSpeed = 2.f;
		break;
	case STATE_SLIDE:
		m_pTransformCom->Set_CurSpeed(m_pTransformCom->Get_CurSpeed() + 4.f);
		break;
	case STATE_SLIDELENDING:
		m_fSlepSpeed = m_pTransformCom->Get_CurSpeed();
		break;
	default:
		m_fSlepSpeed = 0.f;
		break;
	}





	// 애니 갱신
	Set_Anim();

	m_TickStates.clear();
}

void CPlayer::Set_Anim()
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


void CPlayer::Tick(_float fTimeDelta)
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
	case STATE_JUMPLENDING:
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


void CPlayer::Idle_Tick(_float fTimeDelta)
{
	if (STATE_SPRINT == m_ePreState)
	{
		m_bImStop = true;
		m_TickStates.push_back(STATE_SLEP);
	}
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

void CPlayer::Rend_Tick(_float fTimeDelta)
{
	// m_eJumpState = STATE_IDLE;

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
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
			m_eJumpState = STATE_SPRINT;
		else
			m_eJumpState = STATE_WALK;
	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		vTotalLook += -1.f * vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
			m_eJumpState = STATE_RUN;
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
			m_eJumpState = STATE_SPRINT;
		else
			m_eJumpState = STATE_WALK;
	}


	if (pGameInstance->Key_Pressing(DIK_A))
	{
		vTotalLook += -1.f * vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
			m_eJumpState = STATE_RUN;
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
			m_eJumpState = STATE_SPRINT;
		else
			m_eJumpState = STATE_WALK;
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		vTotalLook += vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
			m_eJumpState = STATE_RUN;
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
			m_eJumpState = STATE_SPRINT;
		else
			m_eJumpState = STATE_WALK;
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
			m_eJumpState = STATE_RUN;
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
			m_eJumpState = STATE_SPRINT;
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
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
			m_eJumpState = STATE_SPRINT;
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
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
			m_eJumpState = STATE_SPRINT;
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
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
			m_eJumpState = STATE_SPRINT;
		else
			m_eJumpState = STATE_RUN;
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
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
			m_eJumpState = STATE_SPRINT;
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
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
			m_eJumpState = STATE_SPRINT;
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
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
			m_eJumpState = STATE_SPRINT;
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
		else if (pGameInstance->Key_Pressing(DIK_CAPSLOCK))
			m_eJumpState = STATE_SPRINT;
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
			m_TickStates.push_back(STATE_JUMP);
			m_pTransformCom->Set_CurSpeed(m_fWalkSpeed);
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

#pragma endregion 






#pragma region LateTick


void CPlayer::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	// 중력 적용
	m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 0.008f);

	// 착지 했다면
	_float fTemp = 0.f;
	if (m_pNavigationCom->isGround(m_pTransformCom->Get_State(CTransform::STATE_POSITION), &fTemp) || COBB::COL_ON == Get_StaticOBB()->Get_ColState())
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
		else if (STATE_SLIDE == m_eState)
		{
			m_TickStates.push_back(STATE_SLIDELENDING);
		}

	}


	// 상태 갱신
	Set_State();

	// 이동 갱신
	Calcul_State(fTimeDelta);

	// 여기서 셀에 있는 콜라이더와 충돌처리 확인하고 밀린다.
	Tick_Col(m_pTransformCom->Get_WorldMatrix(), m_pNavigationCom, m_pTransformCom);


	// 애니메이션 END 이벤트
	if (m_pModelCom->Play_Animation(fTimeDelta))
		Check_EndAnim();


	// 소켓 갱신
	m_pSockatCom->Tick(fTimeDelta, m_pTransformCom);
	m_pSockatCom->LateTick(fTimeDelta, m_pRendererCom);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_PLAYER, this);
	RELEASE_INSTANCE(CGameInstance);


}

void CPlayer::Calcul_State(_float fTimeDelta)
{

	if (STATE_WALK == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(m_fWalkSpeed);
	}
	else if (STATE_RUN == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(m_fRunSpeed);
	}
	else if (STATE_SPRINT == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(m_fRunSpeed + 2.f);
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
			m_pTransformCom->Set_CurSpeed(0.1f);
	}
	else if (STATE_JUMP == m_eState || STATE_RUNJUMP == m_eState || STATE_SPRINTJUMP == m_eState || STATE_DOUBLEJUMP == m_eState || STATE_SLIDE == m_eState)
	{	
		// 점프상태로 벽에 부딪혔으면
 		if (COBB::COL_BLOCK == Get_StaticOBB()->Get_ColState())
			m_pTransformCom->Set_CurSpeed(0.f);

	}
	else if (STATE_IDLE == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(0.f);
	}
	else if (STATE_ATTACK_1 == m_eState || STATE_ATTACK_2 == m_eState || STATE_ATTACK_3 == m_eState)
	{
		m_pTransformCom->Set_CurSpeed(0.f);
	}

	m_pTransformCom->Go_Straight(m_pTransformCom->Get_CurSpeed(), fTimeDelta, m_pNavigationCom);
}

void CPlayer::Check_EndAnim()
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


HRESULT CPlayer::Render()
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


	Render_Col();


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



	/* For.Com_Sockat */
	CSockat::SOCATDESC SockatDesc;
	ZeroMemory(&SockatDesc, sizeof(CSockat::SOCATDESC));
	XMStoreFloat4x4(&SockatDesc.mPivot, m_pModelCom->Get_PivotMatrix());
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sockat"), TEXT("Com_Sockat"), (CComponent**)&m_pSockatCom, &SockatDesc)))
		return E_FAIL;

	CSockat::PARTSDESC PartsDesc;
	ZeroMemory(&PartsDesc, sizeof(CSockat::PARTSDESC));
	PartsDesc.vPos =	_float3(0.f, 0.f, -0.63f);
	PartsDesc.vScale =	_float3(1.f, 1.f, 1.f);
	PartsDesc.vRot =	_float3(-90.f, 0, 0);
	m_pSockatCom->Add_Sockat("bip_hat01", m_pModelCom, TEXT("Prototype_GameObject_Ori_Hat"), PartsDesc);

	PartsDesc.vPos = _float3(-0.28f, 0.11f, -0.41f);
	PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
	PartsDesc.vRot = _float3(0.f, 0.f, 178.f);
	m_pSockatCom->Add_Sockat("bip_ItemPalmR01", m_pModelCom, TEXT("Prototype_GameObject_Umbrella"), PartsDesc);





	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.3f, 1.f, 0.3f);
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	if (FAILED(AddCollider(CCollider::TYPE_AABB, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;




	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 6441;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;
	



	return S_OK;
}




#pragma endregion






void CPlayer::OnCollision(CGameObject * pOther)
{
	int i = 0;
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

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTextureCom_SmartEye);
	Safe_Release(m_pSockatCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
