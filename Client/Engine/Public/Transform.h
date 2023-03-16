#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	typedef struct tagTransformDesc
	{
		_float			fSpeedPerSec;
		_float			fRotationPerSec;
	}TRANSFORMDESC;


private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
	}

	_matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_matrix Get_OriScaleWorldMatrix();

	_float4x4 Get_WorldFloat4x4() const {
		return m_WorldMatrix;
	}

	_float4x4 Get_WorldFloat4x4_TP() const {
		_float4x4	WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(Get_WorldMatrix()));
		return WorldMatrix;
	}

	_matrix Get_WorldMatrixInverse() const {
		return XMMatrixInverse(nullptr, Get_WorldMatrix());
	}

	void Set_State(STATE eState, _fvector vState);

	void Set_Look(_fvector vLook);
	void Set_Up(_fvector vUp);

	_float3 Get_PrePos() { return m_vPrePos; }
	_float Get_GravityAcc() { return m_fGravityAcc; }
	_float Get_Velocity() { return m_fVelocity; }

	_float Get_CurSpeed() { return m_fCulSpeed; }
	void Set_CurSpeed(_float fCulSpeed, _bool bLimit = true);

	void Set_Scale(_fvector vScaleInfo);
	_float3 Get_Scale();
	
	void Set_RealOriScale(_fvector vScale) { Set_Scale(vScale); XMStoreFloat3(&m_vOriScale, vScale); }
	void Set_OriScale() { Set_Scale(XMLoadFloat3(&m_vOriScale)); }
	_float3 Get_OriScale() { return m_vOriScale; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Go_Straight(_float fTimeDelta);
	void Go_Straight(_float fSpeedPerSec, _float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta, class CNavigation* pNavigation = nullptr);

	void Go_Scale(_float3 fSpeed, _float fTimeDelta);

	void Turn(_fvector vAxis, _float fTimeDelta);
	void Turn(_fvector vAxis, _float fRotationPerSce, _float fTimeDelta);
	void TurnBack();
	void Turn_Sentor(_fvector vAxis, _fvector vPos, _float fRotationPerSce, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);
	void Rotation(_fvector vAxis1, _float fAngle1, _fvector vAxis2, _float fAngle2);
	void Rotation(_fvector vAxis1, _float fAngle1, _fvector vAxis2, _float fAngle2, _fvector vAxis3, _float fAngle3);
	void RotationStay(_fvector vAxis1, _float fAngle1, _fvector vAxis2, _float fAngle2, _fvector vAxis3, _float fAngle3);

	_bool LinearTurn(_float3 vDestLook, _float fRoationPerSce, _float fDuration, _float fTimeDelta, _bool bCanSlip = true);
	void Set_DestLook();

	void LookAt(_fvector vAt);
	void LookAtInv(_fvector vAt, _float fAnlge);
	void LookAt_ForLandObject(_fvector vAt);
	void MoveTarget_Lend(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, class CNavigation* pNavigation, _float fLimitDistance = 0.1f);
	_bool Move(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimitDistance = 0.1f);

	void Push_Dir(_fvector vDir, _float fDis, class CNavigation* pNavigation = nullptr);

	void Tick_Gravity(_float fTimeDelta, class CNavigation* pNavigation, _float fGravity = 1.f, _float fMagicNum = 0.f, _bool bIsWiap = false);
	void Jump(_float fPower);
	void DoubleJump(_float fPower);
	void ResetGravity();

	// 절반 민다. 양쪽 호출 시 서로 미는게 된다.
	void Push(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, class CNavigation* pNavigation = nullptr);
	void PushXZ(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, class CNavigation* pNavigation = nullptr);

	// 한 쪽만 민다.
	void PushMe(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, class CNavigation* pNavigation = nullptr);
	void PushOther(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, class CNavigation* pNavigation = nullptr);
	void PushMeXZ(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, class CNavigation* pNavigation = nullptr);
	void PushOtherXZ(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, class CNavigation* pNavigation = nullptr);
	void PushMeX(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, class CNavigation* pNavigation = nullptr);


	//====== Anim ======
public:
	void ReSet_AttackedAnim();
	_bool Tick_AttackAnim(_float fTimeDelta);

private:
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc;
	_float					m_fTimeAcc = 0.f;
	_float3					m_vDest;

	_float3					m_vPrePos;


	_float3					m_vOriScale;


	//====== Gravity ======
	_float					m_fGravity = 10.f;
	_float					m_fGravityAcc = 0.f;
	_float					m_fVelocity = 0.f;
	_float					m_fCulSpeed = 0.f;



	//====== Attacked Anim ======
	_float					m_fAttackedTimeAcc = 0.f;
	_float					m_fAttackedAnimAcc = 0.f;
	_bool					m_bAttackedUpDown = false;
	_float					m_fMaxY = 1.7f;
	_float					m_fMinY = 0.4f;
	_float					m_fMaxYAcc = 0.f;
	_float					m_fMinYAcc = 0.f;


public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END