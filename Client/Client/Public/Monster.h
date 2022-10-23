#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CNavigation;
END

BEGIN(Client)

class CMonster final : public CGameObject
{
public:
	enum MONSTER_STATE { MONSTER_IDLE, MONSTER_MOVE, MONSTER_CHASE, MONSTER_ATTACKED, MONSTER_DIE, MONSTER_END };

private:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	void Set_State(MONSTER_STATE eState);
	void Set_Anim();

public:
	void Attacked();

private:
	HRESULT Ready_Components();

	void Tick_Idle(_float fTimeDelta);
	void Tick_Move(_float fTimeDelta);
	void Tick_Chase(_float fTimeDelta);
	void Tick_Attacked(_float fTimeDelta);
	void Tick_Die(_float fTimeDelta);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

private:
	MONSTER_STATE			m_eState = MONSTER_IDLE;
	MONSTER_STATE			m_ePreState = MONSTER_IDLE;


public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END