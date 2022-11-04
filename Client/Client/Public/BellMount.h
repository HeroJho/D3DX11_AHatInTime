#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Monster.h"

BEGIN(Client)

class CBellMount final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_RING_UP, STATE_RING_DOWN, STATE_END };

public:
	typedef struct tagWispDesc
	{
		_float fRatio;
		_float3 vPos;

	}WISPDESC;

private:
	CBellMount(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBellMount(const CBellMount& rhs);
	virtual ~CBellMount() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


public:
	virtual void Attacked(_int iAT) override;


	void Set_Ratio(_uint iRatio) { m_fRatio = iRatio; }

	void Set_AnimLinearData(ANIM_LINEAR_DATA LinearData);


private:
	HRESULT Ready_Components();


private:
	_float	m_fRatio = 0.f;
	_float m_fMaxRatio = 0.f;
	STATE m_eState = STATE_END;

	_uint m_iNaviIndex = 0;

	_float m_fSpeed = 0.f;

	_bool m_bFirstHit = false;

public:
	static CBellMount* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END