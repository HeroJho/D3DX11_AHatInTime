#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "Monster.h"


BEGIN(Client)

class CMad_Crow : public CMonster
{
public:
	enum MONSTER_STATE { MONSTER_IDLE, MONSTER_MOVE, MONSTER_CHASE, MONSTER_ATTACKED, MONSTER_DIE, MONSTER_END };

protected:
	CMad_Crow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMad_Crow(const CMad_Crow& rhs);
	virtual ~CMad_Crow() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	void Set_AnimLinearData(ANIM_LINEAR_DATA LinearData);
	void Set_State(MONSTER_STATE eState);
	void Set_Anim();

public:
	virtual void Attacked(_int iAT);

private:
	virtual HRESULT Ready_Components() override;

	void Tick_Idle(_float fTimeDelta);
	void Tick_Move(_float fTimeDelta);
	void Tick_Chase(_float fTimeDelta);
	void Tick_Attacked(_float fTimeDelta);
	void Tick_Die(_float fTimeDelta);



private:
	MONSTER_STATE			m_eState = MONSTER_IDLE;
	MONSTER_STATE			m_ePreState = MONSTER_IDLE;


	_float					m_fDeadTimeAcc = 0.f;

public:
	static CMad_Crow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END