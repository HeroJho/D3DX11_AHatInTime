#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Monster.h"


BEGIN(Client)

class CSubconEye final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACKED, STATE_DEAD, STATE_END };

private:
	CSubconEye(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSubconEye(const CSubconEye& rhs);
	virtual ~CSubconEye() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;



public:
	virtual void Attacked(_int iAT) override;
	void Dead_Tick(_float fTimeDelta);

	void StartDisApear() { m_bStartDisApear = true; }

private:
	STATE m_eState = STATE_END;
	_bool m_bStartDisApear = false;
	_float m_fAlpa = 1.f;

private:
	virtual HRESULT Ready_Components() override;


public:
	static CSubconEye* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END