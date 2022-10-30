#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Monster.h"


BEGIN(Client)

class CSubSpider final : public CMonster
{
public:
	enum SPIDER_STATE { SPIDER_UPIDLE, SPIDER_DOWNIDLE, SPIDER_DOWN, SPIDER_UP, SPIDER_ATTACK, SPIDER_ATTACKED, SPIDER_DIE, SPIDER_END };

private:
	CSubSpider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSubSpider(const CSubSpider& rhs);
	virtual ~CSubSpider() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	void Set_AnimLinearData(ANIM_LINEAR_DATA LinearData);
	void Set_State(SPIDER_STATE eState);
	void Set_Anim();

public:
	virtual void Attacked(_int iAT) override;

private:
	virtual HRESULT Ready_Components() override;

	void UpIdle_Tick(_float fTimeDelta);
	void DownIdle_Tick(_float fTimeDelta);
	void Down_Tick(_float fTimeDelta);
	void Up_Tick(_float fTimeDelta);
	void Attack_Tick(_float fTimeDelta);
	void Attacked_Tick(_float fTimeDelta);
	void Die_Tick(_float fTimeDelta);



private:
	SPIDER_STATE			m_eState = SPIDER_UPIDLE;
	SPIDER_STATE			m_ePreState = SPIDER_UPIDLE;


	_float3					m_vUpPos;
	_float3					m_vDownPos;

	_float					m_fDeadTimeAcc = 0.f;
	_float					m_fDownIdleTimeAcc = 0.f;
	_bool					m_bOn = false;

public:
	static CSubSpider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END