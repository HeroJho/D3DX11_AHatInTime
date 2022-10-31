#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Item.h"


BEGIN(Engine)

class CNavigation;

END

BEGIN(Client)

class CVault final : public CItem
{
public:
	typedef struct tagVaultDesc
	{
		_uint iNaviIndex = 0;
	}VAULTDESC;

public:
	enum VAULT_STATE { STATE_IDLE, STATE_OPEN, STATE_OPENED, STATE_END };

private:
	CVault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVault(const CVault& rhs);
	virtual ~CVault() = default;



public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT SetUp_State(_fmatrix StateMatrix) override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	void Set_State(VAULT_STATE eState);
	void Set_Anim();



	void Open_Tick(_float fTimeDelta);
	void Opened_Tick(_float fTimeDelta);
	void Idle_Tick(_float fTimeDelta);

	
	virtual void Use_Item() override;


private:
	VAULT_STATE m_eState = STATE_END;
	VAULT_STATE m_ePreState = STATE_END;

	_bool m_bStartOpenning = false;

	_float m_fSprintItemTimeAcc = 0.f;
	_bool m_bIsSprintItem = false;

	_uint m_iNaviIndex = 0;

protected:
	HRESULT Ready_Components();

	virtual _bool Check_OpenIf();

public:
	static CVault* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END