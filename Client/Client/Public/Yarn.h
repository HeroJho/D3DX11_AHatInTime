#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Item.h"


BEGIN(Client)

class CYarn final : public CItem
{
private:
	CYarn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CYarn(const CYarn& rhs);
	virtual ~CYarn() = default;



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



protected:
	HRESULT Ready_Components();


public:
	static CYarn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END