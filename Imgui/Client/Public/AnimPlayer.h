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
class CNavigation;
END

BEGIN(Client)

class CAnimPlayer final : public CGameObject
{
public:
	enum STATE { STATE_IDLE, STATE_WALK, STATE_RUN, STATE_SPRINT, STATE_SLEP, STATE_JUMP, STATE_SLIDE, STATE_JUMPLENDING, STATE_RUNJUMP, STATE_RUNJUMPLENDING, STATE_SLIDELENDING, STATE_SPRINTJUMP, STATE_DOUBLEJUMP, STATE_ATTACK_1, STATE_ATTACK_2, STATE_ATTACK_3, STATE_READYATTACK, STATE_STATU, STATE_END };


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




private:
	void Set_State();
	void Set_Anim();
	void Check_EndAnim();

	void Anim_Face(_float fTimeDelta);

	void Game_Mode(_float fTimeDelta);
	
	void Idle_Tick(_float fTimeDelta);
	void Jump_Tick(_float fTimeDelta);
	void DoubleJump_Tick(_float fTimeDelta);
	void SprintJump_Tick(_float fTimeDelta);
	void Rend_Tick(_float fTimeDelta);
	void Move_Tick(_float fTimeDelta);
	void Slep_Tick(_float fTimeDelta);
	void Slide_Tick(_float fTimeDelta);
	void SlideRending_Tick(_float fTimeDelta);

	void Move_Input(_float fTimeDelta);
	void Attack_Input(_float fTimeDelta);
	void ReadyAttack_Input(_float fTimeDelta);
	void Jump_Input(_float fTimeDelta);
	void DoubleJump_Input(_float fTimeDelta);
	void SprintJump_Input(_float fTimeDelta);
	void Rend_Input(_float fTimeDleta);
	void Slide_Input(_float fTimeDelta);
	void SlideRending_Input(_float fTimeDelta);

	void Calcul_State(_float fTimeDelta);

	HRESULT Choose_Pass(_int iIndex);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CSockat*				m_pSockatCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

private:
	STATE				m_eState = STATE_END;
	STATE				m_ePreState = STATE_END;
	list<STATE>			m_TickStates;
	list<STATE>			m_ComboStates;


	_float3				m_vDestLook;
	_float				m_fCulSpeed = 0.f;
	_float				m_fWalkSpeed = 0.f;
	_float				m_fRunSpeed = 0.f;
	_float				m_fTurnSpeed = 0.f;
	_float				m_fRotationSpeed = 0.f;
	_float				m_fSlepSpeed = 0.f;

	_float				m_fJumpPower = 5.f;
	STATE				m_eJumpState = STATE_END;

	_bool				m_bImStop = false;



	// For. FaceAnim
	_int				m_FaceAnimIndex[2];
	_float				m_fAnimFaceAcc = 0.f;
	_bool				m_bWingk = false;

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







	// For. Tool
public:
	virtual _float3* Get_Axis() override { return &m_vAxis; };

	_float		Get_AnimSpeed(STATE eState);
	void		Set_AnimSpeed(STATE eState, _float fSpeed);

	void Set_AnimLinearData(ANIM_LINEAR_DATA LinearData);
	void Reset_AnimLinearData();

private:
	void Tool_Mode(_float fTimeDelta);

private:
	_float3				m_vAxis;
	_bool				m_bStatu = false;

};

END