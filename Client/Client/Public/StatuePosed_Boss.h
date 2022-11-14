#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Monster.h"


BEGIN(Engine)

class CShader;
class CModel;

END

BEGIN(Client)

class CStatuePosed_Boss final : public CMonster
{
public:
	typedef struct tagStatueDesc
	{
		CGameObject* pTarget = nullptr;
		_float3 vPos;
		_float3 vRotation;
	}STATUEDESC;

public:
	enum STATE { STATE_IDLE_NONE, STATE_IDLE_NONE_T, STATE_IDLE, STATE_IDLE_T, STATE_RUN, STATE_END };

private:
	CStatuePosed_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatuePosed_Boss(const CStatuePosed_Boss& rhs);
	virtual ~CStatuePosed_Boss() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private:
	void Tick_IdleNone(_float fTimeDelta);
	void Tick_IdleNone_T(_float fTimeDelta);
	void Tick_Idle(_float fTimeDelta);
	void Tick_Idle_T(_float fTimeDelta);
	void Tick_Run(_float fTimeDelta);

	void LateTick_None(_float fTimeDelta);
	void LateTick_Anim(_float fTimeDelta);

	void Render_None();
	void Render_Anim();

public:
	void Set_AnimLinearData(ANIM_LINEAR_DATA LinearData);
	void Set_State(STATE eState);
	void Set_Anim();

public:
	virtual void Attacked(_int iAT) override;

private:
	virtual HRESULT Ready_Components() override;

	void Add_PlayerPos(_float fTimeDelta);
	_float3 Pop_PlayerPos();


private:
	STATE			m_eState = STATE_END;
	CGameObject* m_pTarget = nullptr;
	_bool	m_bIsInWisp = false;

	// For. Idle_T
	_float m_fIdle_T_TimeAcc = 0.f;

	// For. Idle_None_T
	_float m_fIdle_None_T_TimeAcc = 0.f;
	_float m_fAlpa = 0.f;

	// For. Run
	list<_float3> m_vPoss;
	_float m_fAddPosTimeAcc = 0.f;
	_float3 m_vNextPos;

private:
	CShader*				m_pShaderCom_None = nullptr;
	CModel*					m_pModelCom_None = nullptr;


public:
	static CStatuePosed_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END