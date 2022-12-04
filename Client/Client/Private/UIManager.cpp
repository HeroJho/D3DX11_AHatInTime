#include "stdafx.h"
#include "..\Public\UIManager.h"
#include "GameInstance.h"


#include "ItemManager.h"
#include "ToolManager.h"


#include "UI_Inven.h"
#include "UI_Item_Inven.h"
#include "UI_DiamondScore.h"
#include "UI_Shop.h"
#include "UI_SpeechBubble.h"
#include "UI_SmallSpeechBubble.h"
#include "UI_Health.h"
#include "WhiteBoard.h"
#include "UI_Loading.h"
#include "CheckPoint.h"


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
	UiInfoDesc.fSizeX = 320.f;
	UiInfoDesc.fSizeY = 100.f;
	UiInfoDesc.fX = g_iWinSizeX * 0.5f;
	UiInfoDesc.fY = 45.85f;
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


	UiInfoDesc.fSizeX = 230.f;
	UiInfoDesc.fSizeY = 142.f;
	UiInfoDesc.fX = g_iWinSizeX * 0.5f;
	UiInfoDesc.fY = 86.7f;
	UiInfoDesc.pDesc = nullptr;


	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_UI_SmallSpeechBubbles"), LEVEL_STATIC, TEXT("Layer_UI"), &pObj, &UiInfoDesc)))
		return E_FAIL;

	m_pSmallSpeechBubble = (CUI_SmallSpeechBubble*)pObj;
	Safe_AddRef(m_pSmallSpeechBubble);


	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CUIManager::Make_Hp()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));
	UiInfoDesc.fSizeX = 150;
	UiInfoDesc.fSizeY = 150;
	UiInfoDesc.fX = 140;
	UiInfoDesc.fY = 110;
	UiInfoDesc.pDesc = nullptr;

	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_UI_Health"), LEVEL_STATIC, TEXT("Layer_UI"), &pObj, &UiInfoDesc)))
		return E_FAIL;

	m_pHp = (CUI_Health*)pObj;
	Safe_AddRef(m_pHp);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUIManager::Make_WhiteBoard()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));
	UiInfoDesc.fSizeX = g_iWinSizeX;
	UiInfoDesc.fSizeY = g_iWinSizeY;
	UiInfoDesc.fX = g_iWinSizeX * 0.5f;
	UiInfoDesc.fY = g_iWinSizeY * 0.5f;
	UiInfoDesc.pDesc = nullptr;

	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_WhiteBoard"), LEVEL_STATIC, TEXT("Layer_UI"), &pObj, &UiInfoDesc)))
		return E_FAIL;

	m_pWhiteBoard = (CWhiteBoard*)pObj;
	Safe_AddRef(m_pWhiteBoard);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUIManager::Make_Loading()
{
	if (nullptr != m_pLoading)
		Safe_Release(m_pLoading);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));
	UiInfoDesc.fSizeX = g_iWinSizeX;
	UiInfoDesc.fSizeY = g_iWinSizeY;
	UiInfoDesc.fX = g_iWinSizeX * 0.5f;
	UiInfoDesc.fY = g_iWinSizeY * 0.5f;
	UiInfoDesc.pDesc = nullptr;

	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_UI_Loading"), LEVEL_STATIC, TEXT("Layer_UI"), &pObj, &UiInfoDesc)))
		return E_FAIL;

	m_pLoading = (CUI_Loading*)pObj;
	Safe_AddRef(m_pLoading);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUIManager::Make_CheckPoint()
{
	if (nullptr != m_pCheckPoint)
		Safe_Release(m_pCheckPoint);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));
	UiInfoDesc.fSizeX = 231.f;
	UiInfoDesc.fSizeY = 136.f;
	UiInfoDesc.fX = 1165.5f;
	UiInfoDesc.fY = 378.f;
	UiInfoDesc.pDesc = nullptr;

	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_CheckPoint"), LEVEL_STATIC, TEXT("Layer_UI"), &pObj, &UiInfoDesc)))
		return E_FAIL;

	m_pCheckPoint = (CCheckPoint*)pObj;
	Safe_AddRef(m_pCheckPoint);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}



void CUIManager::OnOff_DiamondScore(_bool bBool)
{
	m_pDiamondScore->OnOff(bBool);
}

void CUIManager::OnOff_Inven(_bool bBool)
{
	m_pItem_Inven->OnOff(bBool);
}

void CUIManager::OnOff_HP(_bool bBool)
{
	m_pHp->OnOff(bBool);
}

void CUIManager::OnOff_Loading(_bool bBool)
{
	m_pLoading->Set_Start(bBool);
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

void CUIManager::Open_CheckPoint()
{
	if (nullptr == m_pCheckPoint)
		return;
	m_pCheckPoint->Set_OnOff(true);
}

void CUIManager::Close_CheckPoint()
{
	if (nullptr == m_pCheckPoint)
		return;
	m_pCheckPoint->Set_OnOff(false);
}

_uint CUIManager::Get_Hp()
{
	if (nullptr == m_pHp)
		return 10;

	return m_pHp->Get_Hp();
}

void CUIManager::Set_Hp(_uint iHp)
{
	if (nullptr == m_pHp)
		return;

	m_pHp->Set_Hp(iHp);
}



void CUIManager::Set_WhiteBoard(_bool bStart)
{
	m_pWhiteBoard->Set_Start(bStart);
}

void CUIManager::On_Text(TCHAR * sText, _float fSize, _float fPower, _bool bShake, _bool bSmall)
{
	if (bSmall)
	{
		if (nullptr == m_pSmallSpeechBubble)
			return;
		m_pSmallSpeechBubble->On_Text(sText, fSize, fPower, bShake);

	}
	else
	{
		if (nullptr == m_pSpeechBubble)
			return;
		m_pSpeechBubble->On_Text(sText, fSize, fPower, bShake);
	}


}

void CUIManager::Set_Text(TCHAR * sText, _float fSize, _float fPower, _bool bShake, _bool bSmall)
{

	if (bSmall)
	{
		if (nullptr == m_pSmallSpeechBubble)
			return;
		m_pSmallSpeechBubble->Set_Text(sText, fSize, fPower, bShake);
	}
	else
	{
		if (nullptr == m_pSpeechBubble)
			return;
		m_pSpeechBubble->Set_Text(sText, fSize, fPower, bShake);
	}

}

void CUIManager::Off_Text(_bool bSmall)
{
	if (bSmall)
	{
		if (nullptr == m_pSmallSpeechBubble)
			return;
		m_pSmallSpeechBubble->Off_Text();
	}
	else
	{
		if (nullptr == m_pSpeechBubble)
			return;
		m_pSpeechBubble->Off_Text();
	}


}

void CUIManager::Set_Target(CGameObject * pObj)
{
	m_pSmallSpeechBubble->Set_Target(pObj);
}




void CUIManager::Make_WitchChargEffect()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LEVEL eLevel = CToolManager::Get_Instance()->Get_CulLevel();
	pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_WitchChargEffect"), eLevel, TEXT("Layer_Effect"), nullptr);

	RELEASE_INSTANCE(CGameInstance);
}






void CUIManager::Free()
{

	Safe_Release(m_pLoading);
	Safe_Release(m_pCheckPoint);
	Safe_Release(m_pHp);
	Safe_Release(m_pSpeechBubble);
	Safe_Release(m_pSmallSpeechBubble);
	Safe_Release(m_pDiamondScore);
	Safe_Release(m_pInven);
	Safe_Release(m_pItem_Inven);
	Safe_Release(m_pShop);
	Safe_Release(m_pWhiteBoard);
	
}
