#include "stdafx.h"
#include "..\Public\Camera_Free.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free & rhs, CTransform::TRANSFORMDESC * pArg)
	: CCamera(rhs, pArg)
{

}








HRESULT CCamera_Free::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Free::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	

	return S_OK;
}

void CCamera_Free::Tick(_float fTimeDelta)
{

	switch (m_eType)
	{
	case Client::CCamera_Free::CAM_TOOL:
		Tool_Mode(fTimeDelta);
		break;
	case Client::CCamera_Free::CAM_GAME:
		Game_Mode(fTimeDelta);
		break;
	}


	__super::Tick(fTimeDelta);

}

void CCamera_Free::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}








void CCamera_Free::ChangeGameMode(CGameObject * pPlayer)
{
	if (nullptr == pPlayer)
		return;

	m_pPlayer = pPlayer;
	m_fDis = 5.f;
	ZeroMemory(&m_vAngle, sizeof(_float3));

	Safe_AddRef(m_pPlayer);

	m_eType = CAM_GAME;
}

void CCamera_Free::ChangeToolMode()
{
	Safe_Release(m_pPlayer);
	m_eType = CAM_TOOL;
}






void CCamera_Free::Tool_Mode(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Mouse_Pressing(DIMK_RBUTTON))
	{

		if (pGameInstance->Get_DIKState(DIK_W) & 0x80)
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
		}

		if (pGameInstance->Get_DIKState(DIK_S) & 0x80)
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
		}

		if (pGameInstance->Get_DIKState(DIK_A) & 0x80)
		{
			m_pTransformCom->Go_Left(fTimeDelta);
		}

		if (pGameInstance->Get_DIKState(DIK_D) & 0x80)
		{
			m_pTransformCom->Go_Right(fTimeDelta);
		}

	}


	_long	MouseMove = 0;


	if (pGameInstance->Mouse_Pressing(DIMK_RBUTTON))
	{
		if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))
		{
			m_pTransformCom->Turn(XMVECTOR{ 0.f, 1.f, 0.f }, MouseMove * fTimeDelta * 0.05f);
		}

		if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.05f);
		}
	}

	Safe_Release(pGameInstance);
}



void CCamera_Free::Game_Mode(_float fTimeDelta)
{
	if (nullptr == m_pPlayer)
		m_eType = CAM_TOOL;

	Game_Mode_Input(fTimeDelta);


	CTransform* pPlayerTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
	
	_matrix mX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_vAngle.x));
	_matrix mY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_vAngle.y));

	_vector vCamDir = XMVector3TransformNormal(XMVector3Normalize(XMVectorSet(0.f, 1.f, -1.f, 0.f)), mX);
	vCamDir = XMVector3TransformNormal(vCamDir, mY);
	_vector vCamPos = vCamDir * m_fDis;
	_vector vDestPos = pPlayerTran->Get_State(CTransform::STATE_POSITION) + vCamPos;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDestPos);
	m_pTransformCom->LookAt(pPlayerTran->Get_State(CTransform::STATE_POSITION));

}

void CCamera_Free::Game_Mode_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	_long	MouseMove = 0;


	if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))
	{
		m_vAngle.x += MouseMove * fTimeDelta * 2.f;
		if (360.f <= m_vAngle.x)
			m_vAngle.x = 0.f;
		else if (0.f >= m_vAngle.x)
			m_vAngle.x = 360.f;

	}

	if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))
	{
		m_vAngle.y += MouseMove * fTimeDelta * 2.f;
		if (360.f <= m_vAngle.y)
			m_vAngle.y = 0.f;
		else if (0.f >= m_vAngle.y)
			m_vAngle.y = 360.f;
	}



	RELEASE_INSTANCE(CGameInstance);
}








CCamera_Free * CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free*		pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Free::Clone(void* pArg)
{
	CCamera::CAMERADESC*		pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CCamera_Free*		pInstance = new CCamera_Free(*this, &pCameraDesc->TransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();


	Safe_Release(m_pPlayer);

}
