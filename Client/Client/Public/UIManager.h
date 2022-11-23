#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CGameObject;

END

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
	HRESULT Make_SpeechBubble();
	HRESULT Make_Hp();


	// For. Inven
public:
	HRESULT Update_HatInvenSlot();
	HRESULT Update_ItemInvenSlot();

	// For. Diamond
public:
	HRESULT Set_Score(_uint iNum);

	// For. Shop
public:
	void Open_Shop();
	void Close_Shop();

	// For. Hp
public:

	// For. Text
public:
	void On_Text(TCHAR* sText, _float fSize, _float fPower, _bool bShake = false, _bool bSmall = false);
	void Set_Text(TCHAR* sText, _float fSize, _float fPower, _bool bShake = false, _bool bSmall = false);
	void Off_Text(_bool bSmall = false);

	void Set_Target(class CGameObject* pObj);

public:
	void Make_WitchChargEffect();


private:
	class CUI_Inven*		m_pInven = nullptr;
	class CUI_Item_Inven*	m_pItem_Inven = nullptr;
	class CUI_DiamondScore* m_pDiamondScore = nullptr;
	class CUI_Shop*			m_pShop = nullptr;
	class CUI_Health*		m_pHp = nullptr;
	
	class CUI_SpeechBubble*			m_pSpeechBubble = nullptr;
	class CUI_SmallSpeechBubble*	m_pSmallSpeechBubble = nullptr;


public:
	virtual void Free() override;

};

END