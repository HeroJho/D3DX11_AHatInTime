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

	HRESULT Update_HatInvenSlot();
	HRESULT Update_ItemInvenSlot();


private:
	class CUI_Inven*		pInven = nullptr;
	class CUI_Item_Inven*	pItem_Inven = nullptr;

public:
	virtual void Free() override;

};

END