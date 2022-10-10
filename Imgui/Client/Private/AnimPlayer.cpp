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

	return S_OK;
}





#pragma region GET_SET

_float CAnimPlayer::Get_AnimSpeed(STATE eState)
{
	switch (eState)
	{
	case Client::CAnimPlayer::STATE_IDLE:
		return m_pModelCom->Get_Anim_TickPerSecond(111);
	case Client::CAnimPlayer::STATE_WALK:
		return m_pModelCom->Get_Anim_TickPerSecond(197);
	case Client::CAnimPlayer::STATE_RUN:
		return m_pModelCom->Get_Anim_TickPerSecond(198);
	case Client::CAnimPlayer::STATE_SLEP:
		return m_pModelCom->Get_Anim_TickPerSecond(177);
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
	case Client::CAnimPlayer::STATE_SLEP:
		m_fSlepSpeed = 2.f;
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
	{
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
	}
		break;
	case STATE_SPRINT:
		m_pModelCom->Set_AnimIndex(153);
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
		else if (pGameInstance->Key_Pressing(DIK_SPACE))
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
		// XMStoreFloat3(&m_vDestLook, -1.f * vLendLook);
		vTotalLook += -1.f * vLendLook;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vLendLook;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_TickStates.push_back(STATE_RUN);
		}
		else if (pGameInstance->Key_Pressing(DIK_SPACE))
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
		// XMStoreFloat3(&m_vDestLook, -1.f*vCamRight);
		vTotalLook += -1.f * vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += -1.f * vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_TickStates.push_back(STATE_RUN);
		}
		else if (pGameInstance->Key_Pressing(DIK_SPACE))
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
		// XMStoreFloat3(&m_vDestLook, vCamRight);
		vTotalLook += vCamRight;
		if (0.1f > XMVectorGetX(XMVector3Length(vTotalLook)))
			vTotalLook += vCamRight;

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_TickStates.push_back(STATE_RUN);
		}
		else if (pGameInstance->Key_Pressing(DIK_SPACE))
		{
			m_TickStates.push_back(STATE_SPRINT);
		}
		else
		{
			m_TickStates.push_back(STATE_WALK);
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
		m_fSlepSpeed = 2.f;
		m_TickStates.push_back(STATE_SLEP);
	}

}

void CAnimPlayer::Slep_Tick(_float fTimeDelta)
{
	m_fSlepSpeed -= 5.f * fTimeDelta;
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

#pragma endregion 






#pragma region LateTick

void CAnimPlayer::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	// 상태 갱신
	Set_State();

	// 이동 갱신
	Calcul_State(fTimeDelta);

	// 애니메이션 END 이벤트
	if (m_pModelCom->Play_Animation(fTimeDelta))
		Check_EndAnim();


	m_pSockatCom->LateTick(fTimeDelta, m_pRendererCom);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

void CAnimPlayer::Calcul_State(_float fTimeDelta)
{
	if (STATE_WALK == m_eState)
	{
		m_pTransformCom->Go_Straight(m_fWalkSpeed, fTimeDelta);
	}
	else if (STATE_RUN == m_eState)
	{
		m_pTransformCom->Go_Straight(m_fRunSpeed, fTimeDelta);
	}
	else if (STATE_SPRINT == m_eState)
	{
		m_pTransformCom->Go_Straight(m_fRunSpeed + 1.f, fTimeDelta);
	}
	else if (STATE_SLEP == m_eState)
	{
		m_pTransformCom->Go_Straight(m_fSlepSpeed, fTimeDelta);
	}
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
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}

	return S_OK;
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


	Safe_Release(m_pSockatCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
