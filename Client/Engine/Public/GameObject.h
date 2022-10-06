#pragma	 once

#include "Base.h"
#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_ComponentPtr(const _tchar* pComponentTag);
	_float Get_CamDistance() const {
		return m_fCamDistance;
	}

	list<CCollider*> Get_Colliders() { return m_Colliders; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CGameObject* pOther) {};
	
	void Tick_Col(_fmatrix TransformMatrix);
	void Render_Col();

public:
	virtual HRESULT SetUp_State(_fmatrix StateMatrix) { return S_OK; }
	HRESULT AddCollider(CCollider::TYPE eType, CCollider::tagColliderDesc Desc);


protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

protected: /* ��ü���� �߰��� ������Ʈ���� Ű�� �з��Ͽ� �����Ѵ�. */
	map<const _tchar*, class CComponent*>			m_Components;
	typedef map<const _tchar*, class CComponent*>	COMPONENTS;

	// �ݶ��̴� ����
	list<CCollider*>		m_Colliders;


protected:
	_float				m_fCamDistance = 0.f;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	

private:
	class CComponent* Find_Component(const _tchar* pComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END