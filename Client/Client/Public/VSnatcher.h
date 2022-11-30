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
class CSockat;
END

BEGIN(Client)

class CVSnatcher final : public CGameObject
{
public:
	enum STATE {
		STATE_CUT_0, STATE_CUT_1, STATE_CUT_2, STATE_CUT_3, STATE_CUT_4, STATE_CUT_5, STATE_CUT_6, STATE_CUT_7,
		STATE_APPEAR, STATE_SOFTAPPEAR, STATE_DISAPPEAR, STATE_IDLE, 
		STATE_TALKING, STATE_CURSESTART, STATE_CURSE, 
		STATE_MINON, STATE_MAGICSTART, STATE_MAGIC, STATE_HOITSTART, 
		STATE_HOIT, STATE_SNAPHAT, 
		STATE_SWIPSTART, STATE_SWIPS, STATE_SIT,
		STATE_ATTACK, STATE_CANATTACKED, STATE_DEAD, STATE_END };
	enum SLOT { SLOT_HAND, SLOT_HEAD, SLOT_SPIN, SLOT_END };

private:
	CVSnatcher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVSnatcher(const CVSnatcher& rhs);
	virtual ~CVSnatcher() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	void Set_AnimLinearData(ANIM_LINEAR_DATA LinearData);
	void Set_State(STATE eState);
	void Set_Anim();

	void Attacked();

public:
	void Tick_Appear(_float fTimeDelta);
	void Tick_SoftAppear(_float fTimeDelta);
	void Tick_DisAppear(_float fTimeDelta);
	void Tick_Idle(_float fTimeDelta);
	void Tick_Talking(_float fTimeDelta);
	void Tick_CurseStart(_float fTimeDelta);
	void Tick_Curse(_float fTimeDelta);
	void Tick_Minon(_float fTimeDelta);

	void Tick_MagicStart(_float fTimeDelta);
	void Tick_Magic(_float fTimeDelta);

	void Tick_HoItStart(_float fTimeDelta);
	void Tick_HoIt(_float fTimeDelta);

	void Tick_SwipStart(_float fTimeDelta);
	void Tick_Swip(_float fTimeDelta);

	void Tick_Attack(_float fTimeDelta);

	void Tick_SnapHat(_float fTimeDelta);

	void Tick_CanAttacked(_float fTimeDelta);

	void Tick_Dead(_float fTimeDelta);

	void Tick_Cut_1(_float fTimeDelta);
	void Tick_Cut_2(_float fTimeDelta);
	void Tick_Cut_3(_float fTimeDelta);
	void Tick_Cut_4(_float fTimeDelta);
	void Tick_Cut_5(_float fTimeDelta);
	void Tick_Cut_6(_float fTimeDelta);
	void Tick_Cut_7(_float fTimeDelta);

private:
	void Compute_Pattern(_float fTimeDelta);
	void End_Anim();

	void Create_ExPlo(_fvector vPos);
	void Create_Magic(_uint iCount);

	void Create_Statue();
	void Create_CubeBox();

	void Choose_SnapHat();
	void Drop_Hat();


	_float3 Get_PacePos(_float fLength, _float fUpAngle, _float fRightAngle);
	_float3 Get_PaceLook(_float fHight);

	void Start_Dark();
	void End_Dark();



private:
	// For. Common
	STATE			m_eState = STATE_END;
	STATE			m_ePreState = STATE_END;

	class CPlayer*		m_pPlayer = nullptr;
	class CCaulDron*	m_pCaulDron = nullptr;

	_uint m_iNaviIndex = 0;

	// For. Curse
	_float m_fCurseTimeAcc = 0.f;
	_float m_fCurseDelayTime = 1.f;
	_int	m_iCurseCount = 0;
	_int	m_iCurseMaxCount = 5;

	// For. Magic
	_float m_fMagicTimeAcc = 0.f;
	_float m_fMagicEndTime = 10.f;

	// For. HoIt
	_float3 m_vSentorPos;
	_int m_iHoItCount = 0;
	_int m_iHoItMaxCount = 5;
	_bool m_bIsUp = true;
	_float m_fHoItTimeAcc = 0.f;

	// For. Swip
	class CSwip* m_pSwip = nullptr;
	_float m_fSwipTimeAcc = 0.f;

	// For. Minon
	_float m_fMinonTimeAcc = 0.f;

	// For. SnapHat
	_float m_fSnapHatTimeAcc = 0.f;
	_int m_iSnapIndex = -1;
	string   m_sSnapTag = "";
	_uint m_iSnatCount = 0;


	// For. Cut
	_float m_fCutTimeAcc_1 = 0.f;
	_int m_iCutIndex = 0;
	_int m_iTalkCount = 0;
	_bool m_bCutBool = false;

	_bool m_bDark = false;


	// For. CanAttacked
	_float m_fCanAttackedTimeAcc = 0.f;

	_bool m_bAttacked = false;
	_float m_fAttackedTimeAcc = 0.f;

	// For. Dead
	_float m_fDeadTimeAcc = 0.f;
	_float m_fDeadMaxTimeAcc = 1.f;
	_uint  m_iFinLaserCount = 0;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CSockat*				m_pSockatCom = nullptr;

private:
	virtual HRESULT Ready_Components();
	HRESULT Ready_Sockat();

	HRESULT Equip_Sockat(string sItemName, SLOT eSlot);

public:
	static CVSnatcher* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END