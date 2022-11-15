#include "stdafx.h"
#include "..\Public\UIManager.h"
#include "GameInstance.h"


#include "ItemManager.h"

#include "UI_Inven.h"
#include "UI_Item_Inven.h"
#include "UI_DiamondScore.h"
#include "UI_Shop.h"
#include "UI_SpeechBubble.h"



IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
{

}








HRESULT CUIManager::Make_InvenUI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_UI_Inven"), LEVEL_STATIC, TEXT("Layer_UI"), &pObj, nullptr)))
		return E_FAIL;

	m_pInven = (CUI_Inven*)pObj;
	Safe_AddRef(m_pInven);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUIManager::Make_ItemInvenUI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));
	UiInfoDesc.fSizeX = 0;
	UiInfoDesc.fSizeY = 0;
	UiInfoDesc.fX = g_iWinSizeX * 0.5f;
	UiInfoDesc.fY = 100;
	UiInfoDesc.pDesc = nullptr;


	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_UI_Item_Inven"), LEVEL_STATIC, TEXT("Layer_UI"), &pObj, &UiInfoDesc)))
		return E_FAIL;

	m_pItem_Inven = (CUI_Item_Inven*)pObj;
	Safe_AddRef(m_pItem_Inven);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUIManager::Make_DiamondUI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 0.f;
	UiInfoDesc.fSizeY = 0.f;
	UiInfoDesc.fX = 70;
	UiInfoDesc.fY = 680;
	UiInfoDesc.pDesc = nullptr;

	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_UI_DiamondScore"), LEVEL_STATIC, TEXT("Layer_UI"), &pObj, &UiInfoDesc)))
		return E_FAIL;

	m_pDiamondScore = (CUI_DiamondScore*)pObj;
	Safe_AddRef(m_pDiamondScore);


	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CUIManager::Make_ShopUI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 391.7f;
	UiInfoDesc.fSizeY = 391.7;
	UiInfoDesc.fX = 946.96f;
	UiInfoDesc.fY = 228.17f;
	UiInfoDesc.pDesc = nullptr;

	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_UI_Shop"), LEVEL_STATIC, TEXT("Layer_UI"), &pObj, &UiInfoDesc)))
		return E_FAIL;

	m_pShop = (CUI_Shop*)pObj;
	Safe_AddRef(m_pShop);


	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CUIManager::Make_SpeechBubble()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 872.73f;
	UiInfoDesc.fSizeY = 244.75f;
	UiInfoDesc.fX = g_iWinSizeX * 0.5f;
	UiInfoDesc.fY = 86.7f;
	UiInfoDesc.pDesc = nullptr;

	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_UI_SpeechBubble"), LEVEL_STATIC, TEXT("Layer_UI"), &pObj, &UiInfoDesc)))
		return E_FAIL;

	m_pSpeechBubble = (CUI_SpeechBubble*)pObj;
	Safe_AddRef(m_pSpeechBubble);


	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}




HRESULT CUIManager::Update_HatInvenSlot()
{
	if (nullptr == m_pInven)
		return E_FAIL;

	m_pInven->LoadItemMgr_ItemUI();


	return S_OK;
}

HRESULT CUIManager::Update_ItemInvenSlot()
{
	if (nullptr == m_pItem_Inven)
		return E_FAIL;

	m_pItem_Inven->LoadItemMgr_ItemUI();


	return S_OK;
}

HRESULT CUIManager::Set_Score(_uint iNum)
{

	m_pDiamondScore->Set_Score(iNum);

	return S_OK;
}

void CUIManager::Open_Shop()
{
	if (nullptr == m_pShop)
		return;
	m_pShop->Open();
}

void CUIManager::Close_Shop()
{
	if (nullptr == m_pShop)
		return;
	m_pShop->Close();
}

void CUIManager::On_Text(TCHAR * sText, _bool bShake)
{
	if (nullptr == m_pSpeechBubble)
		return;
	m_pSpeechBubble->On_Text(sText, bShake);
}

void CUIManager::Set_Text(TCHAR * sText, _bool bShake)
{
	if (nullptr == m_pSpeechBubble)
		return;
	m_pSpeechBubble->Set_Text(sText, bShake);
}

void CUIManager::Off_Text()
{
	if (nullptr == m_pSpeechBubble)
		return;
	m_pSpeechBubble->Off_Text();
}






void CUIManager::Free()
{
	Safe_Release(m_pSpeechBubble);
	Safe_Release(m_pDiamondScore);
	Safe_Release(m_pInven);
	Safe_Release(m_pItem_Inven);
	Safe_Release(m_pShop);
}
