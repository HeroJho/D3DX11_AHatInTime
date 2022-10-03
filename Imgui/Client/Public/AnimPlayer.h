#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CSockat;
END

BEGIN(Client)

class CAnimPlayer final : public CGameObject
{
public:
	enum STATE { STATE_IDLE, STATE_WALK, STATE_RUN, STATE_SLIP, STATE_STATU, STATE_END };

private:
	CAnimPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimPlayer(const CAnimPlayer& rhs);
	virtual ~CAnimPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual _float3* Get_Axis() override { return &m_vAxis; };
	_float*		Get_WalkSpeed() { return &m_fWalkSpeed; }
	_float*		Get_RunSpeed() { return &m_fRunSpeed; }
	_float*		Get_TurnSpeed() { return &m_fTurnSpeed; }
	_float*		Get_RotationSpeed() { return &m_fRotationSpeed; }

	_float		Get_AnimSpeed(STATE eState);
	void		Set_AnimSpeed(STATE eState, _float fSpeed);

	void Set_AnimLinearData(ANIM_LINEAR_DATA LinearData);
	void Reset_AnimLinearData();


private:
	void Tool_Mode(_float fTimeDelta);
	void Set_State();
	void Set_Anim();



	void Game_Mode(_float fTimeDelta);
	void Game_Input(_float fTimeDelta);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CSockat*				m_pSockatCom = nullptr;

private:
	_float3					m_vAxis;
	STATE				m_eState = STATE_END;
	list<STATE>			m_TickStates;


	_float3				m_vDestLook;
	_float				m_fCulSpeed = 0.f;
	_float				m_fWalkSpeed = 0.f;
	_float				m_fRunSpeed = 0.f;
	_float				m_fTurnSpeed = 0.f;
	_float				m_fRotationSpeed = 0.f;


	_bool				m_bStatu = false;

private:
	HRESULT Ready_Components();

public:
	CGameObject* Add_Sockat(char* pBoneName, _tchar* cName);
	_bool Get_bStatu() { return m_bStatu; }
	void Set_bStatu(_bool bStatu) { m_bStatu = bStatu; }

public:
	static CAnimPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END