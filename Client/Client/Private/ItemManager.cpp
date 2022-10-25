#include "stdafx.h"
#include "..\Public\ItemManager.h"

#include "GameInstance.h"

#include "ToolManager.h"

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


HRESULT CItemManager::Make_Item(const TCHAR* szItemName, LEVEL eLevel, _float3 vPos, _float3 vAngle, _float3 vScale)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	CItem::ITEMDESC ItemDesc;
	ZeroMemory(&ItemDesc, sizeof(CItem::ITEMDESC));
	
	
	lstrcpy(ItemDesc.szModelName, szItemName);
	ItemDesc.vPos = vPos;
	ItemDesc.vScale = vScale;
	ItemDesc.vAngle = vAngle;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Yarn"), eLevel, TEXT("Layer_Item"), &ItemDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
		

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

void CItemManager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
