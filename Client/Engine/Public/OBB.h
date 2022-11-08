#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL COBB final : public CCollider
{
public:
	enum COL_STATE{ COL_NONE, COL_SLIDE, COL_DOWN, COL_BLOCK, COL_ON, COL_END};

public:
	typedef struct tagOBBDesc
	{
		_float3		vCenter;
		_float3		vCenterAxis[3];
		_float3		vAlignAxis[3];
	}OBBDESC;

private:
	typedef struct tagCheckState
	{
		COL_STATE	eState = COL_END;
		_float fDis = 0.f;
		_float3 vPushDir;
		COBB* pModel = nullptr;
	}CHECKSTATE;

protected:
	COBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COBB(const COBB& rhs);
	virtual ~COBB() = default;

public:
	BoundingOrientedBox Get_Collider() {
		return *m_pOBB;
	}

	COL_STATE Get_ColState() { return m_eColState; }
	void Set_ColState(COL_STATE eColState) { m_eColState = eColState; }

	COL_STATE Get_PreColState() { return m_ePreColState; }
	void Set_PreColState(COL_STATE ePreColState) { m_ePreColState = ePreColState; }

	_float3* Get_HillPoss() { return m_vHillPoss; }

	void Set_IsWall(_bool bIsWall) { m_ColliderDesc.bWall = bIsWall; }

public:
	virtual HRESULT Initialize_Prototype(CCollider::TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix, class CNavigation* pNavi = nullptr, class CTransform* pTran = nullptr, _float fMagicNum = 0.f, _bool bIsInWisp = false);
	virtual _bool Collision(CCollider* pTargetCollider);

	_bool	Collision_Cell(_fvector vA, _fvector vB, _fvector vC, _fmatrix TransformMatrix);

public:
	_bool Collision_OBB(CCollider* pTargetCollider, class CTransform* pTran, _float3* Out_fPushDir, _float* Out_fPlanY, _bool bIsConpi = false);
	_bool DontTouchMe(CCollider* pTargetCollider);

	void Compute_Pigi(class CGameObject* pOther, class CNavigation* pNavi,  class CTransform* pTran, _bool bIsInWisp = false);

private:
	OBBDESC Compute_OBBDesc();
	_float Compute_Height(_fvector vPos, _float3* vPoss);
	_bool Compute_LayPlane(_fvector vSentorPos, _fvector vPos, _fvector vDir, _float3* Out_pPoss, _float* Out_fDot, _float* Out_fY);

	_float3 Copute_CellY(_float3 vPos, _float fa, _float fb, _float fc, _float fd);
	_bool Check_Area(_fvector vA, _fvector vB, _fvector vC, _float _vMaxArea = 0.01f);

	_float3 Compute_MaxRad(_fvector vA, _fvector vB, _fvector vC, _float* Out_MaxRad);

	void Compute_State(class CNavigation* pNavi, class CTransform* pTran);

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif // _DEBUG

private:
	BoundingOrientedBox*	m_pOBB = nullptr;
	BoundingOrientedBox*	m_pOriginal_OBB = nullptr;

	COL_STATE				m_ePreColState = COL_NONE;
	COL_STATE				m_eColState = COL_NONE;

	list<CHECKSTATE>			m_eTickColState;
	list<CHECKSTATE>			m_eTickWallColState;

	_float3 m_vHillPoss[3];

	_bool	m_bOnCollision = false;

public:
	static COBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCollider::TYPE eColliderType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;


};

END