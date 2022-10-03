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
	list<STATE>::iterator iter = max_element(m_TickStates.begin(), m_TickStates.end());
	m_eState = (*iter);

	Set_Anim();
	m_TickStates.clear();
}

void CAnimPlayer::Set_Anim()
{
	switch (m_eState)
	{
	case Client::CAnimPlayer::STATE_IDLE:
		m_pModelCom->Set_AnimIndex(111);
		break;
	case Client::CAnimPlayer::STATE_WALK:
		m_pModelCom->Set_AnimIndex(197);
		break;
	case Client::CAnimPlayer::STATE_RUN:
		m_pModelCom->Set_AnimIndex(198);
		break;
	case Client::CAnimPlayer::STATE_SLIP:
		m_pModelCom->Set_AnimIndex(177);
		break;
	case Client::CAnimPlayer::STATE_STATU:
		m_pModelCom->Set_AnimIndex(204);
		break;
	}
}



void CAnimPlayer::Tick(_float fTimeDelta)
{
	m_TickStates.push_back(STATE_IDLE);

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
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	// 1) 키입력을 받는다. 내가 바라봐야할 방향을 결정한다.
	Game_Input(fTimeDelta);

	// 2) 내 Look과 바라볼 방향을 선형보간하여 점점 바라본다.
	if (m_pTransformCom->LinearTurn(m_vDestLook, 1.f, 0.3f, fTimeDelta))
		m_TickStates.push_back(STATE_SLIP);


	RELEASE_INSTANCE(CGameInstance);
}

void CAnimPlayer::Game_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// WD : 카메라 Lend Look으로 Look
	// AD : 카메라 Right로 Look
	_matrix mCamWorld = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	_vector vCamRight = mCamWorld.r[0];
	_vector vCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vLendLook = XMVector3Cross(vCamRight, vCamUp);


	if (pGameInstance->Key_Pressing(DIK_W))
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			XMStoreFloat3(&m_vDestLook, vLendLook);
			m_TickStates.push_back(STATE_RUN);
		}
		else
		{
			XMStoreFloat3(&m_vDestLook, vLendLook);
			m_TickStates.push_back(STATE_WALK);
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			XMStoreFloat3(&m_vDestLook, -1.f * vLendLook);
			m_TickStates.push_back(STATE_RUN);
		}
		else
		{
			XMStoreFloat3(&m_vDestLook, -1.f * vLendLook);
			m_TickStates.push_back(STATE_WALK);
		}
	}


	if (pGameInstance->Key_Pressing(DIK_A))
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			XMStoreFloat3(&m_vDestLook, -1.f*vCamRight);
			m_TickStates.push_back(STATE_RUN);
		}
		else
		{
			XMStoreFloat3(&m_vDestLook, -1.f*vCamRight);
			m_TickStates.push_back(STATE_WALK);
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			XMStoreFloat3(&m_vDestLook, vCamRight);
			m_TickStates.push_back(STATE_RUN);
		}
		else
		{
			XMStoreFloat3(&m_vDestLook, vCamRight);
			m_TickStates.push_back(STATE_WALK);
		}
	}


	RELEASE_INSTANCE(CGameInstance);
}





void CAnimPlayer::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Set_State();
	if (STATE_WALK == m_eState)
	{
		m_pTransformCom->Go_Straight(m_fWalkSpeed, fTimeDelta);
	}
	else if (STATE_RUN == m_eState)
	{
		m_pTransformCom->Go_Straight(m_fRunSpeed, fTimeDelta);
	}


	// 애니메이션 끝
	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		_uint index = m_pModelCom->Get_PreAnimIndex();
		if (177 == index)
		{

		}
	}

	m_pSockatCom->LateTick(fTimeDelta, m_pRendererCom);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

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
