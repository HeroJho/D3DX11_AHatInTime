#include "stdafx.h"
#include "..\Public\UIManager.h"
#include "GameInstance.h"

#include "UI_Inven.h"
#include "UI_Item_Inven.h"
#include "ItemManager.h"


IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
{
}








HRESULT CUIManager::Make_InvenUI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_UI_Inven"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &pObj, nullptr)))
		return E_FAIL;

	pInven = (CUI_Inven*)pObj;
	Safe_AddRef(pInven);

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
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_UI_Item_Inven"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &pObj, &UiInfoDesc)))
		return E_FAIL;

	pItem_Inven = (CUI_Item_Inven*)pObj;
	Safe_AddRef(pItem_Inven);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}




HRESULT CUIManager::Update_HatInvenSlot()
{
	if (nullptr == pInven)
		return E_FAIL;

	pInven->LoadItemMgr_ItemUI();


	return S_OK;
}

HRESULT CUIManager::Update_ItemInvenSlot()
{
	if (nullptr == pItem_Inven)
		return E_FAIL;

	pItem_Inven->LoadItemMgr_ItemUI();


	return S_OK;
}






void CUIManager::Free()
{

	Safe_Release(pInven);
	Safe_Release(pItem_Inven);

}
