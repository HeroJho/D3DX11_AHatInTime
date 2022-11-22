#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_Item_Inven final : public CUI
{
protected:
	CUI_Item_Inven(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Item_Inven(const CUI_Item_Inven& rhs);
	virtual ~CUI_Item_Inven() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	void LoadItemMgr_ItemUI();

	void OnOff(_bool bOnOff) { m_bIsOn = bOnOff; }


protected:
	HRESULT Ready_Components();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	_bool			m_bIsOn = false;
	_float2			m_vOnPos;
	_float2			m_vOffPos;


public:
	static CUI_Item_Inven* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END