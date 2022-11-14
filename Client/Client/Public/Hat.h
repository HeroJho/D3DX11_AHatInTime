#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Item.h"

#include "ItemManager.h"

BEGIN(Client)

class CHat final : public CItem
{
public:


private:
	CHat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHat(const CHat& rhs);
	virtual ~CHat() = default;

private:
	CNavigation* m_pNavigationCom = nullptr;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT SetUp_State(_fmatrix StateMatrix) override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	virtual void Use_Item() override;

public:
	void Init_Pigic_Bounding(_float OriJumpPow, _float OriDirPow);
	void Tick_Pigic_Bounding(_float fTimeDelta);


private:
	CItemManager::SPRINTDESC m_Desc;

	_bool m_bStop = false;

	_bool m_bOn = false;
	_float m_fJumpAcc = 0.f;
	_float m_fJumpPower = 0.f;
	_float m_fDirPower = 0.f;


protected:
	HRESULT Ready_Components();


public:
	static CHat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END