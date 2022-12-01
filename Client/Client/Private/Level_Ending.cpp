#include "stdafx.h"
#include "..\Public\Level_Ending.h"

#include "GameInstance.h"

#include "DataManager.h"
#include "ItemManager.h"
#include "CamManager.h"
#include "UIManager.h"
#include "CutSceneManager.h"

#include "Camera_Free.h"
#include "UI.h"
#include "MonsterVault.h"
#include "BellMount.h"
#include "SpikeBlock.h"
#include "BadgeS_Base.h"
#include "PuzzleCube.h"
#include "RotateBarrel.h"
#include "StatuePosed.h"
#include "TimeObject.h"

#include "Terrain.h"

#include "Player.h"

CLevel_Ending::CLevel_Ending(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Ending::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;


	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;



	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;


	CUIManager::Get_Instance()->OnOff_Loading(false);

	return S_OK;
}

void CLevel_Ending::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	CCutSceneManager::Get_Instance()->Tick(fTimeDelta);

}

HRESULT CLevel_Ending::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	SetWindowText(g_hWnd, TEXT("게임플레이레벨임"));

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Clear_Light();

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.9f, 0.9f, 0.9f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


HRESULT CLevel_Ending::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	//if (FAILED(CDataManager::Get_Instance()->Load_Map(5, LEVEL_BOSS)))   // 3 5
	//	return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);




	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr(LEVEL_STATIC, TEXT("Layer_Player"), 0);
	pPlayer->SetPosNavi(LEVEL_ENDING, XMVectorSet(-60.6f, -0.031f, -90.23f, 1.f));

	pPlayer->Set_RenderSkip(true);


	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Sky"), LEVEL_ENDING, pLayerTag)))
		return E_FAIL;

	CTerrain::TERRAINDESCs TDesc;
	TDesc.eLevel = LEVEL_ENDING;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Terrain"), LEVEL_ENDING, pLayerTag, &TDesc)))
		return E_FAIL;


	CGameObject::CREATUREINFODESC ObjDesc;
	ZeroMemory(&ObjDesc, sizeof(CGameObject::CREATUREINFODESC));
	ObjDesc.iAT = 1;
	ObjDesc.iMaxHP = 3;
	ObjDesc.iHP = 1;
	ObjDesc.vPos = _float3(-62.6f, 0.29f, -116.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_SubconBossEye"), LEVEL_ENDING, TEXT("Layer_Monster"), &ObjDesc)))
		return E_FAIL;





	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_VSnatcher"), LEVEL_ENDING, TEXT("Layer_VSnatcher"))))
		return E_FAIL;



	//CTimeObject::TIMEOBJECTDESC TimeObjDesc;
	//TimeObjDesc.vPos = _float3(-60.f, 3.23f, -115.8f);
	//TimeObjDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//TimeObjDesc.vScale = _float3(1.f, 1.f, 1.f);
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_TimeObject"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &TimeObjDesc)))
	//	return E_FAIL;












	Safe_Release(pGameInstance);


	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));
	UiInfoDesc.fSizeX = 150;
	UiInfoDesc.fSizeY = 150;
	UiInfoDesc.fX = 120;
	UiInfoDesc.fY = 120;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_UI_Health"), LEVEL_GAMEPLAY, pLayerTag, &UiInfoDesc)))
		return E_FAIL;




	Safe_Release(pGameInstance);


	CUIManager::Get_Instance()->Make_InvenUI();
	CUIManager::Get_Instance()->Make_ItemInvenUI();
	CUIManager::Get_Instance()->Make_DiamondUI();
	CUIManager::Get_Instance()->Make_ShopUI();


	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_NPC(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	CBadgeS_Base::BADGES_BASEDESC Desc;
	ZeroMemory(&Desc, sizeof(CBadgeS_Base::BADGES_BASEDESC));
	Desc.vPos = _float3(-72.f, -0.73f, -11.0f);
	Desc.vAngle = _float3(0.f, 0.f, 0.f);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BadgeS_Base"), LEVEL_GAMEPLAY, pLayerTag, &Desc)))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}



CLevel_Ending * CLevel_Ending::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Ending*		pInstance = new CLevel_Ending(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Ending"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Ending::Free()
{
	__super::Free();

}


