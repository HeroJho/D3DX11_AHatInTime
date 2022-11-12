#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

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


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_NPC(TEXT("Layer_Npc"))))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	

	// CCamManager::Get_Instance()->Play_CutScene(0, true);
	CItemManager::Get_Instance()->Add_Hat(TEXT("Ori_Hat"));
	//CItemManager::Get_Instance()->Add_Hat(TEXT("Witch_Hat"));
	//CItemManager::Get_Instance()->Add_Hat(TEXT("Mask_Fox"));
	//CItemManager::Get_Instance()->Add_Hat(TEXT("Mask_Cat"));


	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	CCutSceneManager::Get_Instance()->Tick(fTimeDelta);

}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	SetWindowText(g_hWnd, TEXT("게임플레이레벨임"));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

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

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{

	CCamManager::Get_Instance()->Create_Cam();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	CGameObject::CREATUREINFODESC ObjDesc;
	ZeroMemory(&ObjDesc, sizeof(CGameObject::CREATUREINFODESC));
	ObjDesc.iAT = 1;
	ObjDesc.iMaxHP = 4;
	ObjDesc.iHP = 4;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, pLayerTag, &ObjDesc)))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	if (FAILED(CDataManager::Get_Instance()->Load_Map(5, LEVEL_GAMEPLAY)))   // 3 5
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_VSnatcher"), LEVEL_GAMEPLAY, TEXT("Layer_VSnatcher"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Sky"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Terrain"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Toilet_SentorWall"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	
	//CPuzzleCube::PUZZLECUBEDESC PuzzleCubeDesc;
	//PuzzleCubeDesc.vPos = _float3(-35.7f, 16.f, -8.f);
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PuzzleCube"), LEVEL_GAMEPLAY, pLayerTag, &PuzzleCubeDesc)))
	//	return E_FAIL;
	//PuzzleCubeDesc.vPos = _float3(-46.7f, 5.f, 6.f);
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PuzzleCube"), LEVEL_GAMEPLAY, pLayerTag, &PuzzleCubeDesc)))
	//	return E_FAIL;
	//PuzzleCubeDesc.vPos = _float3(-70.6f, 25.f, -14.8f);
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PuzzleCube"), LEVEL_GAMEPLAY, pLayerTag, &PuzzleCubeDesc)))
	//	return E_FAIL;



	//
	//CSpikeBlock::SPIKEBLOCKDESC SpikeDesc;
	//SpikeDesc.vPos = _float3(-33.136f, 21.577f, 112.661f);
	//SpikeDesc.vScale = _float3(1.f, 1.f, 1.f);
	//SpikeDesc.vRotation = _float3(-17.3f, -4.2f + 90.f, 10.8f);
	//SpikeDesc.vColScale = _float3(8.5f, 2.4f, 2.6f);
	//SpikeDesc.vMyRight = true;
	//SpikeDesc.vAix = _float3(1.f, 0.f, 0.f);
	//SpikeDesc.fSpeed = 0.4f;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_SpikeBlock"), LEVEL_GAMEPLAY, pLayerTag, &SpikeDesc)))
	//	return E_FAIL;
	//SpikeDesc.vPos = _float3(-41.5f, 29.35f, 128.9f);
	//SpikeDesc.vScale = _float3(1.5f, 0.5f, 0.5f);
	//SpikeDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//SpikeDesc.vColScale = _float3(25.4f, 2.f, 0.9f);
	//SpikeDesc.vMyRight = false;
	//SpikeDesc.vAix = _float3(0.f, 1.f, 0.f);
	//SpikeDesc.fSpeed = 0.2f;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_SpikeBlock"), LEVEL_GAMEPLAY, pLayerTag, &SpikeDesc)))
	//	return E_FAIL;

	//SpikeDesc.vPos = _float3(-42.24f, 5.47f, 9.72f);
	//SpikeDesc.vScale = _float3(1.f, 1.f, 1.f);
	//SpikeDesc.vRotation = _float3(0.f, 31.8f, -21.5f);
	//SpikeDesc.vColScale = _float3(8.5f, 2.4f, 2.6f);
	//SpikeDesc.vMyRight = true;
	//SpikeDesc.vAix = _float3(1.f, 0.f, 0.f);
	//SpikeDesc.fSpeed = 2.f;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_SpikeBlock"), LEVEL_GAMEPLAY, pLayerTag, &SpikeDesc)))
	//	return E_FAIL;
	//CRotateBarrel::ROTATEBARRELDESC RotateDesc;
	//RotateDesc.vPos = _float3(-62.43f, 16.68f, 13.52f);
	//RotateDesc.vRotation = _float3(-39.9f, -2.9f, 78.7f);
	//RotateDesc.fSpeed = 5.f;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_RotateBarrel"), LEVEL_GAMEPLAY, pLayerTag, &RotateDesc)))
	//	return E_FAIL;




	//if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("Prototype_GameObject_Yarn"), TEXT("yarn_ui_sprint"), LEVEL_GAMEPLAY, _float3(-40.75f, 15.34f, 157.85f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f))))
	//	return E_FAIL;
	//if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("Prototype_GameObject_Yarn"), TEXT("yarn_ui_ice"), LEVEL_GAMEPLAY, _float3(-41.75f, 15.34f, 157.85f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f))))
	//	return E_FAIL;
	//if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("Prototype_GameObject_Yarn"), TEXT("yarn_ui_hover"), LEVEL_GAMEPLAY, _float3(-42.75f, 15.34f, 157.85f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f))))
	//	return E_FAIL;
	//if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("Prototype_GameObject_Yarn"), TEXT("yarn_ui_brew"), LEVEL_GAMEPLAY, _float3(-43.75f, 15.34f, 157.85f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f))))
	//	return E_FAIL;
	//if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("Prototype_GameObject_Yarn"), TEXT("Umbrella"), LEVEL_GAMEPLAY, _float3(-44.75f, 15.34f, 157.85f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f))))
	//	return E_FAIL;

	//



	//if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("Prototype_GameObject_MonsterVault"), TEXT("vault"), LEVEL_GAMEPLAY, _float3(-10.43f, 8.f, 118.47f), _float3(0.f, 0.f, 0.f), _float3(1.f, 1.f, 1.f), 1)))
	//	return E_FAIL;


	//if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("Prototype_GameObject_Vault"), TEXT("vault"), LEVEL_GAMEPLAY, _float3(-21.02f, 32.53f, 126.75f), _float3(0.f, 90.f, 0.f), _float3(1.f, 1.f, 1.f), 1)))
	//	return E_FAIL;

	//if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("Prototype_GameObject_Vault_2"), TEXT("vault"), LEVEL_GAMEPLAY, _float3(-35.11f, 32.59f, 149.31f), _float3(0.f, 90.f, 0.f), _float3(1.f, 1.f, 1.f), 1)))
	//	return E_FAIL;


	//if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("Prototype_GameObject_Vault_State"), TEXT("vault"), LEVEL_GAMEPLAY, _float3(-44.3f, -0.74f, 3.19f), _float3(0.f, 0.f, 0.f), _float3(1.f, 1.f, 1.f), 1)))
	//	return E_FAIL;

	


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);



	


	CTimeObject::TIMEOBJECTDESC TimeObjDesc;
	TimeObjDesc.vPos = _float3(-60.f, 3.23f, -115.8f);
	TimeObjDesc.vRotation = _float3(0.f, 0.f, 0.f);
	TimeObjDesc.vScale = _float3(1.f, 1.f, 1.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_TimeObject"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &TimeObjDesc)))
		return E_FAIL;


	CBellMount::WISPDESC WispDesc;
	WispDesc.vPos = _float3(-62.4f, 4.42f, -25.11f);
	WispDesc.vAngle = _float3(0.f, 103.1f, 0.f);
	WispDesc.fRatio = 70;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BellMount"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &WispDesc)))
		return E_FAIL;
	WispDesc.vPos = _float3(-41.75f, 6.43f, 83.f);
	WispDesc.vAngle = _float3(0.f, 0.f, 0.f);
	WispDesc.fRatio = 70;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BellMount"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &WispDesc)))
		return E_FAIL;
	WispDesc.vPos = _float3(-72.62f, 15.3f, -14.7f);
	WispDesc.vAngle = _float3(0.f, 146.9f + 180.f, 0.f);
	WispDesc.fRatio = 40;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BellMount"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &WispDesc)))
		return E_FAIL;
	WispDesc.vPos = _float3(-28.0f, 1.4f, 44.84f);
	WispDesc.vAngle = _float3(0.f, 0.f, 0.f);
	WispDesc.fRatio = 20;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BellMountEye"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &WispDesc)))
		return E_FAIL;





	CStatuePosed::STATUEDESC StatueDesc;
	CGameObject* pObj = nullptr;
	StatueDesc.vPos = _float3(-54.7f, -0.83f, 3.9f);
	StatueDesc.vRotation = _float3(0.f, -122.2f, 0.f);
	StatueDesc.pTarget = pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StatuePosed"), LEVEL_GAMEPLAY, pLayerTag, &pObj, &StatueDesc)))
		return E_FAIL;
	StatueDesc.vPos = _float3(-62.14f, -1.0f, -2.21f);
	StatueDesc.vRotation = _float3(0.f, 26.1f, 0.f);
	StatueDesc.pTarget = pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StatuePosed"), LEVEL_GAMEPLAY, pLayerTag, &pObj, &StatueDesc)))
		return E_FAIL;
	StatueDesc.vPos = _float3(-55.9f, -.4f, -17.95f);
	StatueDesc.vRotation = _float3(0.f, -15.1f, 0.f);
	StatueDesc.pTarget = pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StatuePosed"), LEVEL_GAMEPLAY, pLayerTag, &pObj, &StatueDesc)))
		return E_FAIL;




	
	CGameObject::CREATUREINFODESC ObjDesc;
	ZeroMemory(&ObjDesc, sizeof(CGameObject::CREATUREINFODESC));
	ObjDesc.iAT = 1;
	ObjDesc.iMaxHP = 3;
	ObjDesc.iHP = 1;


	ObjDesc.vPos = _float3(-27.9f, 0.148f, 45.04f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_SubconEye"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &ObjDesc)))
		return E_FAIL;



	ObjDesc.vPos = _float3(-11.9f, 22.2f, 93.417f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_SubSpider"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &ObjDesc)))
		return E_FAIL;
	ObjDesc.vPos = _float3(-15.18f, 24.72f, 108.52f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_SubSpider"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &ObjDesc)))
		return E_FAIL;
	ObjDesc.vPos = _float3(-17.7f, 26.22f, 114.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_SubSpider"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &ObjDesc)))
		return E_FAIL;
	ObjDesc.vPos = _float3(-19.45f, 28.9f, 120.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_SubSpider"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &ObjDesc)))
		return E_FAIL;
	ObjDesc.vPos = _float3(-44.2f, 9.85f, -27.3f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_SubSpider"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &ObjDesc)))
		return E_FAIL;
	ObjDesc.vPos = _float3(-79.f, 9.87f, -.9f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_SubSpider"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &ObjDesc)))
		return E_FAIL;
	

	Safe_Release(pGameInstance);


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
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

HRESULT CLevel_GamePlay::Ready_Layer_NPC(const _tchar * pLayerTag)
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



CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}


