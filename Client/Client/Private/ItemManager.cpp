#include "stdafx.h"
#include "..\Public\ItemManager.h"

#include "GameInstance.h"

#include "ToolManager.h"
#include "UIManager.h"

#include "Yarn.h"
#include "Flask.h"

#include "Player.h"



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


HRESULT CItemManager::Make_Item(const TCHAR* szObjName, const TCHAR* szItemName, LEVEL eLevel, _float3 vPos, _float3 vAngle, _float3 vScale, _uint iCount, void* pArg)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	CItem::ITEMDESC ItemDesc;
	ZeroMemory(&ItemDesc, sizeof(CItem::ITEMDESC));
	
	
	lstrcpy(ItemDesc.szModelName, szItemName);
	ItemDesc.vPos = vPos;
	ItemDesc.vScale = vScale;
	ItemDesc.vAngle = vAngle;
	ItemDesc.iCount = iCount;
	ItemDesc.pDesc = pArg;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(szObjName, eLevel, TEXT("Layer_Item"), &ItemDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
		

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CItemManager::Make_DrowItem(const TCHAR * szObjName, const TCHAR * szItemName, LEVEL eLevel, _float3 vPos, _float3 vAngle, _float3 vScale, _uint iItemCount, _float3 vSprDir, _float fSprDirPow, _float fSprJumpPow, _uint iNaviIndex)
{
	SPRINTDESC Desc;
	Desc.bIsPigic = true;
	Desc.vDir = vSprDir;
	Desc.fDirPow = fSprDirPow;
	Desc.fJumpPow = fSprJumpPow;
	Desc.iNaviIndex = iNaviIndex;
	
	if (FAILED(CItemManager::Get_Instance()->Make_Item(szObjName, szItemName, eLevel, vPos, vAngle, vScale, iItemCount, &Desc)))
		return E_FAIL;

	return S_OK;
}
HRESULT CItemManager::Make_PopSprintItem(const TCHAR * szObjName, const TCHAR * szItemName, LEVEL eLevel, _float3 vPos, _float3 vAngle, _float3 vScale, _uint iItemCount, _uint iNaviIndex, _uint iSprCount)
{


	for (_uint i = 0; i < iSprCount; ++i)
	{
		_float fDegree = CToolManager::Get_Instance()->Get_RendomNum(0.f, 360.f);
		_float3 vDir;
		XMStoreFloat3(&vDir, XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMMatrixRotationY(XMConvertToRadians(fDegree))));	
		_float fJumpPow = CToolManager::Get_Instance()->Get_RendomNum(3.f, 8.f);
		_float fDirPow = CToolManager::Get_Instance()->Get_RendomNum(4.f, 6.f);

		SPRINTDESC Desc;
		Desc.bIsPigic = true;
		Desc.vDir = vDir;
		Desc.fDirPow = fDirPow;
		Desc.fJumpPow = fJumpPow;
		Desc.iNaviIndex = iNaviIndex;

		if (FAILED(CItemManager::Get_Instance()->Make_Item(szObjName, szItemName, eLevel, vPos, vAngle, vScale, iItemCount, &Desc)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CItemManager::Make_Hat(TCHAR * pHatModelName, TCHAR* pItemModelName)
{
	for (auto& pItem : m_pItems)
	{
		if (!lstrcmp(pItem.szModelName, pItemModelName) && 0 < pItem.iCount)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
			RELEASE_INSTANCE(CGameInstance);

			pPlayer->Get_Hat(pHatModelName);
			pItem.iCount -= 1;

			CUIManager::Get_Instance()->Update_ItemInvenSlot();

			return S_OK;
		}
	}



	return S_OK;
}



void CItemManager::Change_Hat(TCHAR * pHatModelName)
{
	pHatModelName = Match_TextureWithModelName(pHatModelName);

	for (auto& Hat : m_pHats)
	{
		if (!lstrcmp(Hat.szModelName, pHatModelName))
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
			RELEASE_INSTANCE(CGameInstance);

			char temp[MAX_PATH];
			CToolManager::Get_Instance()->TCtoC(pHatModelName, temp);
			pPlayer->Equip_Sockat(temp, CPlayer::SLOT_HAT);

			return;
		}
	}
}

HRESULT CItemManager::Make_Flask(_fvector vPos, _fvector vDir, _float fDirPow, _float fJumpPow, _uint iNaviIndex)
{
	CFlask::FLASKDESC Desc;

	_float3 vVPos;
	XMStoreFloat3(&vVPos, vPos);
	XMStoreFloat3(&Desc.vDir, vDir);
	Desc.fDirPow = fDirPow;
	Desc.fJumpPow = fJumpPow;
	Desc.iNaviIndex = iNaviIndex;

	vVPos.y += 0.5f;
	CItemManager::Get_Instance()->Make_Item(TEXT("Prototype_GameObject_Flask"), TEXT("science_owlbrew_remade"), LEVEL_GAMEPLAY, vVPos, _float3(0.f, 0.f, 0.f), _float3(1.f, 1.f, 1.f), 1, &Desc);

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




TCHAR* CItemManager::Match_TextureWithModelName(TCHAR* pTextureName)
{


	if (!lstrcmp(pTextureName, TEXT("Prototype_Component_Texture_Icon_SprintHat")) || !lstrcmp(pTextureName, TEXT("Sprint_Hat")))
	{
		return TEXT("Sprint_Hat");
	}
	else if (!lstrcmp(pTextureName, TEXT("Prototype_Component_Texture_Icon_WitchHat")) || !lstrcmp(pTextureName, TEXT("Witch_Hat")))
	{
		return TEXT("Witch_Hat");
	}
	else if (!lstrcmp(pTextureName, TEXT("Prototype_Component_Texture_Icon_KidHat")) || !lstrcmp(pTextureName, TEXT("Ori_Hat")))
	{
		return TEXT("Ori_Hat");
	}

}

