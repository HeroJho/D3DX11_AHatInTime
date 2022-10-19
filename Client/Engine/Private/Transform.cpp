#include "..\Public\Transform.h"

#include "Navigation.h"
#include "Collider.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{

}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_matrix	WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	WorldMatrix.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

void CTransform::Set_Look(_fvector vLook)
{
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Set_CurSpeed(_float fCulSpeed, _bool bLimit)
{
	m_fCulSpeed = fCulSpeed;
	if (0.f > m_fCulSpeed && bLimit)
		m_fCulSpeed = 0.f;
}


HRESULT CTransform::Initialize_Prototype()
{
	/* vector -> float : XMStore*/
	/* float -> vector : XMLoad */

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	m_vDest = _float3{ 0.f, 0.f, 1.f };

	return S_OK;
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Straight(_float fSpeedPerSec, _float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * fSpeedPerSec * fTimeDelta;

	/* �����̾ �����̰� �� ������ ��ġ�� �׺���̼ǿ� �����Ͽ�. */
	/* ���� ��Ȳ���� ������ �� �ֈd�� üũ�Ѵ�. */
	_bool		isMove = true;

	if (nullptr != pNavigation)
		isMove = pNavigation->isMove(vPosition);


	if (true == isMove)
	{
		XMStoreFloat3(&m_vPrePos, Get_State(STATE_POSITION));
		Set_State(STATE_POSITION, vPosition);
	}

}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta)
{
	_vector vPosition = Get_State(CTransform::STATE_POSITION);
	_vector vDirN = XMVector3Normalize(vDir);

	vPosition += vDir * fSpeed * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}


void CTransform::Set_Scale(_fvector vScaleInfo)
{
	if (0.001f > XMVectorGetX(vScaleInfo) || 0.001f > XMVectorGetY(vScaleInfo) || 0.001f > XMVectorGetZ(vScaleInfo))
		return;

	Set_State(CTransform::STATE_RIGHT,
		XMVector3Normalize(Get_State(CTransform::STATE_RIGHT)) * XMVectorGetX(vScaleInfo));
	Set_State(CTransform::STATE_UP,
		XMVector3Normalize(Get_State(CTransform::STATE_UP)) * XMVectorGetY(vScaleInfo));
	Set_State(CTransform::STATE_LOOK,
		XMVector3Normalize(Get_State(CTransform::STATE_LOOK)) * XMVectorGetZ(vScaleInfo));
}

_float3 CTransform::Get_Scale()
{
	return _float3(
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_UP))),
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_LOOK))));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * fTimeDelta);

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}
void CTransform::Turn(_fvector vAxis, _float fRotationPerSce, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRotationPerSce * fTimeDelta);

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}

void CTransform::TurnBack()
{
	_vector vMyLook = Get_State(STATE_LOOK);
	vMyLook *= -1.f;
	XMStoreFloat3(&m_vDest, vMyLook);
	Set_DestLook();
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	// Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(60.0f));

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	_float3		Scale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f) * Scale.x, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f) * Scale.y, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f) * Scale.z, RotationMatrix));
}
void CTransform::Rotation(_fvector vAxis1, _float fAngle1, _fvector vAxis2, _float fAngle2)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis1, XMConvertToRadians(fAngle1));
	_float3		Scale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f) * Scale.x, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f) * Scale.y, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f) * Scale.z, RotationMatrix));

	RotationMatrix = XMMatrixRotationAxis(vAxis2, XMConvertToRadians(fAngle2));
	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));

}
void CTransform::Rotation(_fvector vAxis1, _float fAngle1, _fvector vAxis2, _float fAngle2, _fvector vAxis3, _float fAngle3)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis1, XMConvertToRadians(fAngle1));
	_float3		Scale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f) * Scale.x, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f) * Scale.y, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f) * Scale.z, RotationMatrix));

	RotationMatrix = XMMatrixRotationAxis(vAxis2, XMConvertToRadians(fAngle2));
	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));

	RotationMatrix = XMMatrixRotationAxis(vAxis3, XMConvertToRadians(fAngle3));
	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}


_bool CTransform::LinearTurn(_float3 vDestLook, _float fRoationPerSce, _float fDuration, _float fTimeDelta, _bool bCanSlip)
{
	// �̻��� ũ���� (0)���Ͱ� ���� �ÿ�
	_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vDestLook)));
	if (0.01f > fDistance)
		return false;

	if (!bCanSlip)
	{
		_vector vVDestLook = XMVector3Normalize(XMLoadFloat3(&vDestLook));
		_vector vLook = -1.f * XMVector3Normalize(Get_State(STATE_LOOK));
		_float vDist = XMVectorGetX(XMVector3Dot(vVDestLook, vLook));
		if (0.99f < vDist)
			return false;
	}


	_vector vMyLook = Get_State(STATE_LOOK);
	_vector vVDestLook = XMLoadFloat3(&vDestLook);
	_vector vPreDest = XMLoadFloat3(&m_vDest);
	vMyLook = XMVector3Normalize(vMyLook);
	vVDestLook = XMVector3Normalize(vVDestLook);
	vPreDest = XMVector3Normalize(vPreDest);

	// ���� ��ǥ�� �ٸ���
	fDistance = XMVectorGetX(XMVector3Length(vPreDest - vVDestLook));
	if (0.001f < fDistance)
		m_fTimeAcc = 0.f;

	m_fTimeAcc += fRoationPerSce * fTimeDelta;
	_float fRatio = m_fTimeAcc / fDuration;
	if (1.f < fRatio)
	{
		m_fTimeAcc = 0.f;
		return false;
	}



	_vector vCalCulLook = vVDestLook * fRatio + vMyLook * (1.f - fRatio);
	_float fSlip = XMVectorGetX(XMVector3Length(vCalCulLook));
	if (0.8f > fSlip)
	{
		m_vDest = vDestLook;
		return true;
	}



	m_vDest = vDestLook;
	Set_Look(vCalCulLook);

	return false;
}

void CTransform::Set_DestLook()
{
	Set_Look(XMLoadFloat3(&m_vDest));
}

void CTransform::LookAt(_fvector vAt)
{
	_vector		vLook = vAt - Get_State(CTransform::STATE_POSITION);

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_ForLandObject(_fvector vAt)
{
	_vector		vLook = vAt - Get_State(CTransform::STATE_POSITION);



	_float3		vScale = Get_Scale();

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;

	vLook = XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f))) * vScale.z;


	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y);
	Set_State(CTransform::STATE_LOOK, vLook);

}


void CTransform::MoveTarget_Lend(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, CNavigation* pNavigation, _float fLimitDistance)
{
	_vector		vDirection = vTargetPos - Get_State(CTransform::STATE_POSITION);

	_float		fDistance = XMVectorGetX(XMVector3Length(vDirection));

	if (fDistance > fLimitDistance)
	{
		LookAt_ForLandObject(vTargetPos);
		Go_Straight(fSpeed, fTimeDelta, pNavigation);
	}
}

_bool  CTransform::Move(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimitDistance)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vDirection = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDirection));

	if (fDistance < fLimitDistance)
		return true;

	vPosition += XMVector3Normalize(vDirection) * fSpeed * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
	return false;
}

void CTransform::Push_Dir(_fvector vDir, _float fDis, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition += vDir*fDis;

	// CurCell �ٽ� ����
	if (nullptr != pNavigation)
		pNavigation->isMove(vPosition);

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Tick_Gravity(_float fTimeDelta, CNavigation* pNavigation, _float fGravity)
{
	_float3 vPos;
	XMStoreFloat3(&vPos, Get_State(CTransform::STATE_POSITION));

	_float fCellY = 0.f;
	if (pNavigation->isGround(Get_State(CTransform::STATE_POSITION), &fCellY) && (1.f > m_fVelocity))
	{
		m_fGravityAcc = 0.f;
		m_fVelocity = 0.f;
		vPos.y = fCellY;
	}
	else
	{
		m_fGravityAcc += fGravity;
		if (0.5f < m_fGravityAcc)
			m_fGravityAcc = 0.5f;
		m_fVelocity -= m_fGravityAcc;
	}

	vPos.y += m_fVelocity * fTimeDelta;

	m_vPrePos.y += vPos.y;
	Set_State(STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
}

void CTransform::Jump(_float fPower)
{
	m_fVelocity += fPower;
}

void CTransform::DoubleJump(_float fPower)
{
	m_fGravityAcc = 0.f;
	m_fVelocity = fPower;
}

void CTransform::ResetGravity()
{
	m_fGravityAcc = 0.f;
	m_fVelocity = 0.f;
	// m_fCulSpeed = 0.f;
}


CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*			pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*			pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CTransform::Free()
{
	__super::Free();
}
