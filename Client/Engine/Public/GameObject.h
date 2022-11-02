#pragma	 once

#include "Base.h"
#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagCreatureInfoDesc
	{
		_int iHP;
		_int iMaxHP;
		_int iAT;

		_float3 vPos;
		_uint iNaviIndex;

	}CREATUREINFODESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_ComponentPtr(const _tchar* pComponentTag);
	_float Get_CamDistance() const {
		return m_fCamDistance;
	}

	string Get_Tag() { return m_sTag; }

	CCollider* Get_Colliders(string sTag);
	list<CCollider*>* Get_Colliders() { return &m_Colliders; }
	class COBB* Get_StaticOBB();

	CGameObject* Get_Owner() { return m_pOwner; }
	void		Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner;	}

	_bool Get_Dead() { return m_bDead; }
	void Set_Dead(_bool bDead) { m_bDead = bDead; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) {};
	
	void Tick_Col(_fmatrix TransformMatrix, class CNavigation* pNavi = nullptr, class CTransform* pTran = nullptr, _float fMagicNum = 0.f);
	void Render_Col();

public:
	virtual HRESULT SetUp_State(_fmatrix StateMatrix) { return S_OK; }
	HRESULT AddCollider(CCollider::TYPE eType, CCollider::COLLIDERDESC Desc);


protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

protected: /* 객체에게 추가된 컴포넌트들을 키로 분류하여 보관한다. */
	map<const _tchar*, class CComponent*>			m_Components;
	typedef map<const _tchar*, class CComponent*>	COMPONENTS;

	// 콜라이더 보관
	list<CCollider*>		m_Colliders;


protected:
	string			m_sTag = "";
	CGameObject*	m_pOwner = nullptr;
	_float				m_fCamDistance = 0.f;

	_bool			m_bDead = false;
	
	// For.Creature
protected:
	CREATUREINFODESC m_CreatureDesc;


protected:
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	

private:
	class CComponent* Find_Component(const _tchar* pComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END