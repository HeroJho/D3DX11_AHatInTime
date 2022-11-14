#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Item.h"

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

class CPlayer final : public CGameObject
{
public:
	enum STATE { STATE_IDLE, STATE_WALK, STATE_RUN, STATE_SPRINT, STATE_SLEP, STATE_JUMP, STATE_SLIDE, STATE_JUMPLENDING, STATE_RUNJUMP, STATE_RUNJUMPLENDING, STATE_SLIDELENDING, STATE_SPRINTJUMP, STATE_DOUBLEJUMP, STATE_ATTACK_1, STATE_ATTACK_2, STATE_ATTACK_3, STATE_READYATTACK, STATE_JUMPATTACK, STATE_JUMPATTACKRENDING, STATE_HILLDOWN, STATE_STARTGETITEM, STATE_IDLEGETITEM, STATE_ENDGETITEM, STATE_MAGEIDLE, STATE_MAGERUN, STATE_MAGEJUMP, STATE_MAGEIDLEJUMPRENDING, STATE_MAGERUNJUMPRENDING, STATE_MAGEDROW, STATE_ATTACKED, STATE_TALK, STATE_NONE, STATE_APPEAR, STATE_END };
	enum SLOT { SLOT_HAT, SLOT_HAND, SLOT_END };

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	void OnDipY();

public:
	void Set_AnimLinearData(ANIM_LINEAR_DATA LinearData);

	STATE Get_State() { return m_eState; }
	STATE Get_PreState() { return m_ePreState; }

	_bool Get_Attacked() { return m_bAttacked; }

	void Set_Talk() { m_TickStates.push_back(STATE_TALK); }

private:
	void Set_State();
	void Set_Anim();

	void Anim_Face(_float fTimeDelta);
	void Check_Attacked(_float fTimeDelta);

	void Idle_Tick(_float fTimeDelta);
	void Jump_Tick(_float fTimeDelta);
	void DoubleJump_Tick(_float fTimeDelta);
	void SprintJump_Tick(_float fTimeDelta);
	void IdleRend_Tick(_float fTimeDelta);
	void Rend_Tick(_float fTimeDelta);
	void Move_Tick(_float fTimeDelta);
	void Slep_Tick(_float fTimeDelta);
	void Slide_Tick(_float fTimeDelta);
	void SlideRending_Tick(_float fTimeDelta);
	void HillDown_Tick(_float fTimeDelta);
	void JumpAttack_Tick(_float fTimeDelta);
	void Attacked_Tick(_float fTimeDelta);

	void StartGetItem_Tick(_float fTimeDelta);
	void IdleGetItem_Tick(_float fTimeDelta);
	void EndGetItem_Tick(_float fTimeDelta);

	void MageIdle_Tick(_float fTimeDelta);
	void MageRun_Tick(_float fTimeDelta);
	void MageJump_Tick(_float fTimeDelta);
	void MageDrow_Tick(_float fTimeDelta);

	void FoxMask_Tick(_float fTimeDelta);

	void Talk_Tick(_float fTimeDelta);




	void State_Input(_float fTimeDelta);
	
	void Idle_Input(_float fTimeDelta);
	void Move_Input(_float fTimeDelta);
	void Attack_Input(_float fTimeDelta);
	void ReadyAttack_Input(_float fTimeDelta);
	void Jump_Input(_float fTimeDelta);
	void DoubleJump_Input(_float fTimeDelta);
	void SprintJump_Input(_float fTimeDelta);
	void Rend_Input(_float fTimeDleta);
	void Slide_Input(_float fTimeDelta);
	void SlideRending_Input(_float fTimeDelta);
	void HillDown_Input(_float fTimeDelta);
	void JumpAttack_Input(_float fTimeDelta);

	void StartGetItem_Input(_float fTimeDelta);
	void IdleGetItem_Input(_float fTimeDelta);
	void EndGetItem_Input(_float fTimeDelta);

	void MageIdle_Input(_float fTimeDelta);
	void MageRun_Input(_float fTimeDelta);
	void MageJump_Input(_float fTimeDelta);
	void MageDrow_Input(_float fTimeDelta);

	void Talk_Input(_float fTimeDelta);

	void Check_EndAnim();
	void Calcul_State(_float fTimeDelta);


	HRESULT Choose_Pass(_int iIndex);


public:
	void Get_Item(CItem::ITEMINVENDESC Desc);
	void Get_Hat(TCHAR* szModelName, _bool bAction = false);

	HRESULT Equip_Sockat(string sItemName, SLOT eSlot);

	void Attacked();


	CGameObject* Get_NearstMonster() { return m_pNearstMonster; }
	void Find_NearstMonster();

	void SlowSpeed(_float fSlowSpeed) { m_fSlowSpeed = fSlowSpeed; }

	void SetPosNavi(LEVEL eLevel, _fvector vPos);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CSockat*				m_pSockatCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

	CTexture*				m_pTextureCom_SmartEye = nullptr;

private:
	_float3					m_vAxis;
	STATE				m_eState = STATE_END;
	STATE				m_ePreState = STATE_END;
	list<STATE>			m_TickStates;
	list<STATE>			m_ComboStates;


	_float3				m_vDestLook;
	_float				m_fWalkSpeed = 0.f;
	_float				m_fRunSpeed = 0.f;
	_float				m_fTurnSpeed = 0.f;
	_float				m_fRotationSpeed = 0.f;
	_float				m_fSlepSpeed = 0.f;
	_float				m_fSprintSpeed = 0.f;

	_float				m_fJumpPower = 5.f;
	STATE				m_eJumpState = STATE_END;

	_bool				m_bImStop = false;

	// For. Attacked
	_float				m_fAttackedTimeAcc = 0.f;
	_float				m_fAttackedBoolTimeAcc = 0.f;
	_bool				m_bAttacked = false;

	// For. HillDown
	_float				m_fHillDownTimeAcc = 0.f;
	_float				m_fHillUpTimeAcc = 0.f;

	_float				m_fHillDownSpeed = 0.f;
	_float3				m_vHillDir;

	// For. Mage
	_float				m_fMageGageTime = 2.f;
	_float				m_fMageTimeAcc = 0.f;

	// For. FoxMask
	_float				m_fFoxMaskTimeAcc = 0.f;
	_bool				m_bFoxMask = false;

	// For. FaceAnim
	_int				m_FaceAnimIndex[2];
	_float				m_fAnimFaceAcc = 0.f;
	_bool				m_bWingk = false;

	// For. DeBuff
	_float				m_fSlowSpeed = 1.f;

	// For. NearMonster
	vector<CGameObject*> m_pNearMonsters;
	CGameObject* m_pNearstMonster = nullptr;


	// For. Wisp
	class CWisp* m_pWisp = nullptr;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Sockat();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END