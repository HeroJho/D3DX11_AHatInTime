#include "stdafx.h"
#include "..\Public\ItemManager.h"

#include "GameInstance.h"

#include "ToolManager.h"
#include "UIManager.h"

#include "Yarn.h"



IMPLEMENT_SINGLETON(CItemManager)

CItemManager::CItemManager()
{
}


HRESULT CItemManager::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);

	return S_OK;
}


HRESULT CItemManager::Make_Item(const TCHAR* szObjName, const TCHAR* szItemName, LEVEL eLevel, _float3 vPos, _float3 vAngle, _float3 vScale, _uint iCount)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	CItem::ITEMDESC ItemDesc;
	ZeroMemory(&ItemDesc, sizeof(CItem::ITEMDESC));
	
	
	lstrcpy(ItemDesc.szModelName, szItemName);
	ItemDesc.vPos = vPos;
	ItemDesc.vScale = vScale;
	ItemDesc.vAngle = vAngle;
	ItemDesc.iCount = iCount;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(szObjName, eLevel, TEXT("Layer_Item"), &ItemDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
		

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}





void CItemManager::Add_Hat(TCHAR* szItemName)
{
	if (4 < m_pHats.size())
		return;

	// 중복 체크
	for (auto& pDesc : m_pHats)
	{
		if (!lstrcmp(szItemName, pDesc.szModelName))
			return;
	}


	HATINFODESC Desc;
	ZeroMemory(&Desc, sizeof(HATINFODESC));

	lstrcpy(Desc.szModelName, szItemName);

	m_pHats.push_back(Desc);

	// 인벤을 갱신한다
	CUIManager::Get_Instance()->Update_HatInvenSlot();
}

void CItemManager::Add_Item(TCHAR* szItemName, _uint iCount)
{


	// 중복 체크
	for (auto& pDesc : m_pItems)
	{
		// 중복되면 증가한다
		if (!lstrcmp(szItemName, pDesc.szModelName))
		{
			pDesc.iCount += iCount;
			CUIManager::Get_Instance()->Update_ItemInvenSlot();
			return;
		}
	}


	ITEMINFODESC Desc;
	ZeroMemory(&Desc, sizeof(ITEMINFODESC));

	lstrcpy(Desc.szModelName, szItemName);
	Desc.iCount = iCount;

	m_pItems.push_back(Desc);

	// 인벤을 갱신한다
	CUIManager::Get_Instance()->Update_ItemInvenSlot();
}

void CItemManager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
