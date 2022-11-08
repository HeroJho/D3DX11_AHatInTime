#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CUIManager final : public CBase
{
	DECLARE_SINGLETON(CUIManager)

private:
	CUIManager();
	virtual ~CUIManager() = default;

public:
	HRESULT Make_InvenUI();
	HRESULT Make_ItemInvenUI();
	HRESULT Make_DiamondUI();
	HRESULT Make_ShopUI();

	// For. Inven
public:
	HRESULT Update_HatInvenSlot();
	HRESULT Update_ItemInvenSlot();

	// For. Diamond
public:
	HRESULT Set_Score(_uint iNum);


public:
	void Open_Shop();
	void Close_Shop();


private:
	class CUI_Inven*		m_pInven = nullptr;
	class CUI_Item_Inven*	m_pItem_Inven = nullptr;
	class CUI_DiamondScore* m_pDiamondScore = nullptr;
	class CUI_Shop*			m_pShop = nullptr;

public:
	virtual void Free() override;

};

END