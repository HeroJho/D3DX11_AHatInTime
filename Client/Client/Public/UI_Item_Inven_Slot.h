#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_Item_Inven_Slot final : public CUI
{
public:
	typedef struct tagItemButtonDesc
	{
		TCHAR pIconTag[MAX_PATH];
	}ITEMBUTTONDESC;

private:
	CUI_Item_Inven_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Item_Inven_Slot(const CUI_Item_Inven_Slot& rhs);
	virtual ~CUI_Item_Inven_Slot() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_Count(_uint iCount) { m_iCount = iCount; }

	const TCHAR* Get_ItemModelTag() { return m_pItemModelName; }


private:
	void Match_TextureWithModelName();


private:
	HRESULT Ready_Components();

	virtual void Handle_Hover() override;
	virtual void Handle_Down() override;
	virtual void Handle_Press() override;
	virtual void Handle_Click() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	TCHAR m_pIconTag[MAX_PATH];
	TCHAR m_pItemModelName[MAX_PATH];
	TCHAR m_pHatModelName[MAX_PATH];

	_uint m_iCount = 0;

	_float m_fOriSize = 0.f;
	_float m_fHoverSize = 0.f;
	_bool m_bSound = false;

public:
	static CUI_Item_Inven_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END