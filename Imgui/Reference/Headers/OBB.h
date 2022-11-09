#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL COBB final : public CCollider
{
public:
	enum COL_STATE { COL_NONE, COL_ON, COL_DOWN, COL_BLOCK, COL_END };

public:
	typedef struct tagOBBDesc
	{
		_float3		vCenter;
		_float3		vCenterAxis[3];
		_float3		vAlignAxis[3];
	}OBBDESC;

protected:
	COBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COBB(const COBB& rhs);
	virtual ~COBB() = default;

public:
	BoundingOrientedBox Get_Collider() {
		return *m_pOBB;
	}

	COL_STATE Get_ColState() { return m_eColState; }


	void  Set_Clicked() { m_bClicked = true; }


public:
	virtual HRESULT Initialize_Prototype(CCollider::TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix, class CNavigation* pNavi = nullptr, class CTransform* pTran = nullptr);
	virtual _bool Collision(CCollider* pTargetCollider);

	_bool	Collision_Cell(_fvector vA, _fvector vB, _fvector vC, _fmatrix TransformMatrix);

public:
	_bool Collision_OBB(CCollider* pTargetCollider, class CTransform* pTran, _float3* Out_fPushDir, _float* Out_fPlanY);

private:
	OBBDESC Compute_OBBDesc();
	_float Compute_Height(_fvector vPos);
	_float Compute_Height(_fvector vPos, _float3* vPoss);
	_bool Compute_LayPlane(_fvector vPos, _fvector vDir, _float3* Out_pPoss);

	_float3 Compute_MaxRad(_fvector vA, _fvector vB, _fvector vC, _float* Out_MaxRad);

	virtual void Edit_Col(COLLIDERDESC Desc) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif // _DEBUG

private:
	BoundingOrientedBox*	m_pOBB = nullptr;
	BoundingOrientedBox*	m_pOriginal_OBB = nullptr;

	COL_STATE				m_eColState = COL_NONE;

	_bool m_bClicked = false;

public:
	static COBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCollider::TYPE eColliderType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;


};

END