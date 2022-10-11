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

	m_fCulSpeed = m_fWalkSpeed;

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



	// 애니메이션이 바뀔때 거쳐야하는 부분
	switch (m_eState)
	{
	case STATE_SLEP:
		m_fSlepSpeed = 2.f;
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

void CPlayer::Slep_Tick(_float fTimeDelta)
{
	m_fSlepSpeed -= 6.f * fTimeDelta;
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



void CPlayer::Anim_Face(_float fTimeDelta)
{
	if(0 != m_FaceAnimIndex[0])
		m_fAnimFaceAcc += fTimeDelta * 200.f;
	else
		m_fAnimFaceAcc += fTimeDelta * 0.5f;

	if (1.f < m_fAnimFaceAcc)
	{
		if (m_bWingk)
			--m_FaceAnimIndex[0];
		else
			++m_FaceAnimIndex[0];

		if (m_FaceAnimIndex[1]-1 <= m_FaceAnimIndex[0] || 0 >= m_FaceAnimIndex[0])
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

	// NOTE : Tick으로 올리면 Trun할 때 문제 생김
	// 상태 갱신
	Set_State();
	// 이동 갱신
	Calcul_State(fTimeDelta);
	// 애니메이션 END 이벤트
	if (m_pModelCom->Play_Animation(fTimeDelta))
		Check_EndAnim();


	// Set_State를 한 후에 갱신 한다.
	Tick_Col(m_pTransformCom->Get_WorldMatrix());

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

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	if (FAILED(AddCollider(CCollider::TYPE_AABB, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
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

	Safe_Release(m_pTextureCom_SmartEye);
	Safe_Release(m_pSockatCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
