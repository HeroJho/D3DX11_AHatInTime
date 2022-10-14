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

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Go_Straight(_float fTimeDelta);
	void Go_Straight(_float fSpeedPerSec, _float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta);

	void Set_Scale(_fvector vScaleInfo);
	_float3 Get_Scale();

	void Turn(_fvector vAxis, _float fTimeDelta);
	void Turn(_fvector vAxis, _float fRotationPerSce, _float fTimeDelta);
	void TurnBack();
	void Rotation(_fvector vAxis, _float fRadian);
	void Rotation(_fvector vAxis1, _float fAngle1, _fvector vAxis2, _float fAngle2);
	void Rotation(_fvector vAxis1, _float fAngle1, _fvector vAxis2, _float fAngle2, _fvector vAxis3, _float fAngle3);

	_bool LinearTurn(_float3 vDestLook, _float fRoationPerSce, _float fDuration, _float fTimeDelta);
	void Set_DestLook();

	void LookAt(_fvector vAt);
	void LookAt_ForLandObject(_fvector vAt);
	void MoveTarget_Lend(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, class CNavigation* pNavigation, _float fLimitDistance = 0.1f);
	_bool Move(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimitDistance = 0.1f);

	void Tick_Gravity(_float fTimeDelta, class CNavigation* pNavigation, _float fGravity = 1.f);
	void Jump(_float fPower);


private:
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc;
	_float					m_fTimeAcc = 0.f;
	_float3					m_vDest;

	//====== Gravity ======
	_float					m_fGravity = 10.f;
	_float					m_fGravityAcc = 0.f;
	_float					m_fVelocity = 0.f;


public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END