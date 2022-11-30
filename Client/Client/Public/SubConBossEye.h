#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Monster.h"

BEGIN(Engine)
class CSockat;
END
BEGIN(Client)

class CSubconBossEye final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACKED, STATE_DEAD, STATE_END };
	enum SLOT { SLOT_HAND, SLOT_HEAD, SLOT_SPIN, SLOT_END };

private:
	CSubconBossEye(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSubconBossEye(const CSubconBossEye& rhs);
	virtual ~CSubconBossEye() = default;

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
	CSockat*				m_pSockatCom = nullptr;

	STATE m_eState = STATE_END;
	_bool m_bStartDisApear = false;
	_float m_fAlpa = 1.f;
	_bool m_bDrw = false;

private:
	virtual HRESULT Ready_Components() override;
	HRESULT Ready_Sockat();
	HRESULT Equip_Sockat(string sItemName, SLOT eSlot);


public:
	static CSubconBossEye* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END