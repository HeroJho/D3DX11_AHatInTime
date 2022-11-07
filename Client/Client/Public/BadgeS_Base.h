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

class CBadgeS_Base : public CGameObject
{
public:
	typedef struct tagBadgeS_BaseDesc
	{
		_float3 vPos;
		_float3 vAngle;
	}BADGES_BASEDESC;

	enum STATE { STATE_IDLE, STATE_TALK, STATE_OPEN, STATE_ATTACKED, STATE_END };


protected:
	CBadgeS_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBadgeS_Base(const CBadgeS_Base& rhs);
	virtual ~CBadgeS_Base() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


public:
	void Idle_Tick(_float fTimeDelta);
	void Talk_Tick(_float fTimeDelta);
	void Open_Tick(_float fTimeDelta);
	void Attacked_Tick(_float fTimeDelta);


public:
	void Set_AnimLinearData(ANIM_LINEAR_DATA LinearData);

	virtual void Attacked(_int iAT);

private:
	virtual HRESULT Ready_Components();

	void Set_State(STATE eState);
	void Set_Anim();


protected:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;


private:
	STATE m_eState = STATE_END;
	
	CGameObject* m_pPlayer = nullptr;

public:
	static CBadgeS_Base* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END