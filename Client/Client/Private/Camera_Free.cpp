#include "stdafx.h"
#include "..\Public\Camera_Free.h"
#include "GameInstance.h"

#include "CamManager.h"

#include "Player.h"

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

	m_fOriDis = m_fDis = 4.5f;
	ZeroMemory(&m_vAngle, sizeof(_float3));
	ZeroMemory(&m_vDestLookPos, sizeof(_float3));
	ZeroMemory(&m_vPreLookPos, sizeof(_float3));

	return S_OK;
}

void CCamera_Free::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr == m_pPlayer)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		m_pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr(LEVEL_STATIC, TEXT("Layer_Player"), 0);
		Safe_AddRef(m_pPlayer);


		CTransform* pPlayerTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
		// XmStoreFloat3(m_vPreLookPos

		RELEASE_INSTANCE(CGameInstance);
	}




	switch (m_eState)
	{
	case CAM_GAME:
	case CAM_FOCUS:
	case CAM_BOSS:
	case CAM_FREE:
		Game_Mode(fTimeDelta);
		break;
	case CAM_CUTSCENE:
		CutScene_Mode(fTimeDelta);
		break;
	}




}

void CCamera_Free::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::Set_Pos(_float3 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
}

_bool CCamera_Free::Move(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimitDistance)
{
	return m_pTransformCom->Move(vTargetPos, fSpeed, fTimeDelta, fLimitDistance);
}

void CCamera_Free::LookAt(_fvector vTargetPos)
{
	m_pTransformCom->LookAt(vTargetPos);
}



void CCamera_Free::Look_Player()
{
	CTransform* pPlayerTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
	// 플레이어 발바닥 보다 살짝 위로 본다/
	Set_Target(XMVectorSetY(pPlayerTran->Get_State(CTransform::STATE_POSITION), XMVectorGetY(pPlayerTran->Get_State(CTransform::STATE_POSITION)) + 0.8f));

}

void CCamera_Free::Look_NearMonster()
{
	m_pPlayer->Find_NearstMonster();
	CGameObject* pNearstMonster = m_pPlayer->Get_NearstMonster();
	if (nullptr == pNearstMonster)
	{
		m_eState = CAM_GAME;
		return;
	}

	_vector vPos = ((CTransform*)pNearstMonster->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

	Set_Target(XMVectorSetY(vPos, XMVectorGetY(vPos) + 0.5f));

	Dis_TwoPos(((CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION), vPos);
}

void CCamera_Free::Dis_TwoPos(_fvector vPosL, _fvector vPosR)
{
	_float fDis = XMVectorGetX(XMVector3Length(vPosL - vPosR));

	m_fDis = m_fOriDis - 3.f + fDis * 0.8f;
}

void CCamera_Free::Look_Target()
{
	if (nullptr == m_pTarget)
		return;

	m_pPlayer->Find_NearstMonster();
	CGameObject* pNearstMonster = m_pTarget;


	_vector vPos = ((CTransform*)pNearstMonster->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

	Set_Target(XMVectorSetY(vPos, XMVectorGetY(vPos) + 0.5f));

	Dis_TwoPos(((CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION), vPos);

}
void CCamera_Free::FocuseTarget(_float fDeltaTime)
{
	if (nullptr == m_pTarget)
		return;


	CGameObject* pNearstMonster = m_pTarget;
	if (nullptr == pNearstMonster)
		return;

	CTransform* pPlayerTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
	_vector vPlayerPos = pPlayerTran->Get_State(CTransform::STATE_POSITION);
	_vector vNearstMonsterPos = ((CTransform*)pNearstMonster->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(vPlayerPos - vNearstMonsterPos);


	// x, y 회전 행렬
	_matrix mX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_vAngle.x));
	_vector vCamDirY = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f), mX));

	_float fDot = XMVectorGetX(XMVector3Dot(vDir, vCamDirY));
	// 둔각이다
	if (0.f > fDot)
	{
		_matrix mX = XMMatrixRotationAxis(XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_vAngle.x));
		vCamDirY = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f), mX));
	}

	// 멀어질 방향
	_vector vCamDir = XMVector3Normalize(vCamDirY + vDir);
	// X, Y회전

	_vector vCamPos = vCamDir * m_fDis;
	_vector vDestPos = pPlayerTran->Get_State(CTransform::STATE_POSITION) + vCamPos;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDestPos);

}




void CCamera_Free::Game_Mode(_float fTimeDelta)
{

	switch (m_eState)
	{
	case CAM_GAME:
		m_fDis = m_fOriDis;
		Game_Mode_Input(fTimeDelta);
		Look_Player();
		OriCamPos(fTimeDelta);
		SmoothLook(fTimeDelta);
		break;
	case CAM_FOCUS:
		OriCamPos(fTimeDelta);
		Look_NearMonster();
		Game_Mode_Input(fTimeDelta);
		SmoothLook(fTimeDelta);
		break;
	case CAM_BOSS:
		Boss_Mode_Input(fTimeDelta);
		Look_Target();
		FocuseTarget(fTimeDelta);
		SmoothLook(fTimeDelta);
		break;
	case CAM_FREE:
		SmoothPos(fTimeDelta);
		break;
	}


}

void CCamera_Free::OriCamPos(_float fDeltaTime)
{
	CTransform* pPlayerTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));

	// x, y 회전 행렬
	_matrix mX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_vAngle.x));
	_matrix mY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_vAngle.y));

	// 멀어질 방향
	_vector vCamDir = XMVector3Normalize(XMVectorSet(0.f, 1.f, -1.f, 0.f));

	// X, Y회전
	vCamDir = XMVector3TransformNormal(vCamDir, mX);
	vCamDir = XMVector3TransformNormal(vCamDir, mY);
	_vector vCamPos = vCamDir * m_fDis;
	_vector vDestPos = pPlayerTran->Get_State(CTransform::STATE_POSITION) + vCamPos;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDestPos);
}

void CCamera_Free::FocusePos(_float fDeltaTime)
{
	m_pPlayer->Find_NearstMonster();
	CGameObject* pNearstMonster = m_pPlayer->Get_NearstMonster();
	if (nullptr == pNearstMonster)
	{
		m_eState = CAM_GAME;
		return;
	}

	CTransform* pPlayerTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
	_vector vPlayerPos = pPlayerTran->Get_State(CTransform::STATE_POSITION);
	_vector vNearstMonsterPos = ((CTransform*)pNearstMonster->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(vPlayerPos - vNearstMonsterPos);
	
	// 멀어질 방향
	_vector vCamDir = XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f) + vDir);


	_vector vCamPos = vCamDir * m_fDis;
	_vector vDestPos = pPlayerTran->Get_State(CTransform::STATE_POSITION) + vCamPos;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDestPos);
}

void CCamera_Free::SmoothLook(_float fDeltaTime)
{
	_vector vDestPos = XMLoadFloat3(&m_vDestLookPos);
	_vector vPrePos = XMLoadFloat3(&m_vPreLookPos);

	_vector vDir = vDestPos - vPrePos;
	_vector vNorDir = XMVector3Normalize(vDir);
	_float fDis = XMVectorGetX(XMVector3Length(vDir));

	vPrePos += vNorDir * fDis * 5.f * fDeltaTime;


	_float fX = XMVectorGetX(vPrePos) + XMVectorGetX(vNorDir) * fDis * 3.f * fDeltaTime;
	_float fY = XMVectorGetY(vPrePos) + XMVectorGetY(vNorDir) * fDis * 20.f * fDeltaTime;

	vPrePos = XMVectorSetX(vPrePos, fX);
	vPrePos = XMVectorSetY(vPrePos, fY);

	
	if (0.01f < fDis)
	{
		XMStoreFloat3(&m_vPreLookPos, vPrePos);
		m_pTransformCom->LookAt(XMVectorSetW(vPrePos, 1.f));
	}
	else
	{
		XMStoreFloat3(&m_vPreLookPos, vDestPos);
		m_pTransformCom->LookAt(XMVectorSetW(vDestPos, 1.f));
	}

}

void CCamera_Free::SmoothPos(_float fDeltaTime)
{
	_vector vDestPos = XMLoadFloat3(&m_vDestPos);
	_vector vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_vector vDir = vDestPos - vCurPos;
	_float fDis = XMVectorGetX(XMVector3Length(vDir));
	_vector vNorDir = XMVector3Normalize(vDir);

	if (0.1f > fDis)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vDestPos, 1.f));
	else
		m_pTransformCom->Go_Dir(vNorDir, 3.f, fDeltaTime);
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



	if (pGameInstance->Mouse_Down(DIMK_RBUTTON))
	{
		if (CAM_FOCUS != m_eState)
			m_eState = CAM_FOCUS;
		else if(CAM_FOCUS == m_eState)
		{
 			m_eState = CAM_GAME;
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

	}


	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Free::Boss_Mode_Input(_float fTimeDelta)
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

	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Free::Boss_Mode_Rotation(_float fTimeDelta)
{
	

}

void CCamera_Free::Set_CamFreeValue(_float3 vPos, _float3 vLookPos, _bool bIsFirst)
{
	Set_State(CAM_FREE);
	m_vDestPos = vPos;

	if (bIsFirst)
		m_vPreLookPos = vLookPos;
	m_vDestLookPos = vLookPos;

}


void CCamera_Free::CutScene_Mode(_float fTimeDelta)
{

	CCamManager::Get_Instance()->Tick(fTimeDelta);

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
