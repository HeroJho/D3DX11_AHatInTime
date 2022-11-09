#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Item.h"


BEGIN(Engine)

class CNavigation;

END

BEGIN(Client)

class CFlask final : public CItem
{
public:
	typedef struct tagFlaskDesc
	{
		_float3 vDir;
		_float fDirPow;
		_float fJumpPow;
		_uint iNaviIndex;

	}FLASKDESC;

private:
	CFlask(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFlask(const CFlask& rhs);
	virtual ~CFlask() = default;



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


private:
	CNavigation* m_pNavigationCom = nullptr;


private:
	FLASKDESC m_Desc;
	_bool m_bOn = false;

	_bool m_bEx = false;

protected:
	HRESULT Ready_Components();


public:
	static CFlask* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END