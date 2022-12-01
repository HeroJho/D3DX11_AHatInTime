#include "stdafx.h"
#include "..\Public\Level_Boss.h"

#include "GameInstance.h"

#include "DataManager.h"
#include "ItemManager.h"
#include "CamManager.h"
#include "UIManager.h"
#include "CutSceneManager.h"
#include "ToolManager.h"

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
#include "Fire.h"

#include "Terrain.h"

#include "Player.h"

CLevel_Boss::CLevel_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Boss::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	//	return E_FAIL;


	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	////if (FAILED(Ready_Layer_NPC(TEXT("Layer_Npc"))))
	////	return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
	//	return E_FAIL;



	// CCamManager::Get_Instance()->Play_CutScene(0, true);
	//CItemManager::Get_Instance()->Add_Hat(TEXT("Ori_Hat"));
	//CItemManager::Get_Instance()->Add_Hat(TEXT("Witch_Hat"));
	//CItemManager::Get_Instance()->Add_Hat(TEXT("Mask_Fox"));
	//CItemManager::Get_Instance()->Add_Hat(TEXT("Mask_Cat"));

	CUIManager::Get_Instance()->OnOff_Loading(false);

	return S_OK;
}

void CLevel_Boss::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	CCutSceneManager::Get_Instance()->Tick(fTimeDelta);

}

HRESULT CLevel_Boss::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	SetWindowText(g_hWnd, TEXT("게임플레이레벨임"));

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Lights()
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


HRESULT CLevel_Boss::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	//if (FAILED(CDataManager::Get_Instance()->Load_Map(5, LEVEL_BOSS)))   // 3 5
	//	return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	



	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr(LEVEL_STATIC, TEXT("Layer_Player"), 0);
	pPlayer->SetPosNavi(LEVEL_BOSS, XMVectorSet(-60.6f, -0.031f, -90.23f, 1.f));




	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Sky"), LEVEL_BOSS, pLayerTag)))
		return E_FAIL;

	CTerrain::TERRAINDESCs TDesc;
	TDesc.eLevel = LEVEL_BOSS;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Terrain"), LEVEL_BOSS, pLayerTag, &TDesc)))
		return E_FAIL;






	CFire::FIREDESC FireDesc;
	FireDesc.vPos = _float3(-54.75f, 4.612f + 0.5f, -127.7f);
	FireDesc.vScale = _float3(2.f, 2.f, 2.f);
	FireDesc.fPower = CToolManager::Get_Instance()->Get_RendomNum(10.f, 15.f);
	FireDesc.fUpSpeed = CToolManager::Get_Instance()->Get_RendomNum(0.1f, 0.5f);
	FireDesc.fDownSpeed = FireDesc.fUpSpeed;
	FireDesc.vDiffuseColor = _float4(1.f, 1.f, 5.f, 1.f);
	FireDesc.vAmColor = _float4(0.3f, 0.3f, 0.3f, 1.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Fire"), LEVEL_BOSS, TEXT("Layer_Fire"), &FireDesc)))
		return E_FAIL;
	FireDesc.vPos = _float3(-56.2f, 3.646f + 0.3f, -128.6f);
	FireDesc.vScale = _float3(1.2f, 1.2f, 1.2f);
	FireDesc.fPower = CToolManager::Get_Instance()->Get_RendomNum(10.f, 15.f);
	FireDesc.fUpSpeed = CToolManager::Get_Instance()->Get_RendomNum(0.1f, 0.5f);
	FireDesc.fDownSpeed = FireDesc.fUpSpeed;
	FireDesc.vDiffuseColor = _float4(1.f, 1.f, 5.f, 1.f);
	FireDesc.vAmColor = _float4(0.3f, 0.3f, 0.3f, 1.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Fire"), LEVEL_BOSS, TEXT("Layer_Fire"), &FireDesc)))
		return E_FAIL;
	FireDesc.vPos = _float3(-49.04f, 4.313f + 0.1f, -122.95f);
	FireDesc.vScale = _float3(0.6f, 0.6f, 0.6f);
	FireDesc.fPower = CToolManager::Get_Instance()->Get_RendomNum(10.f, 15.f);
	FireDesc.fUpSpeed = CToolManager::Get_Instance()->Get_RendomNum(0.1f, 0.5f);
	FireDesc.fDownSpeed = FireDesc.fUpSpeed;
	FireDesc.vDiffuseColor = _float4(1.f, 1.f, 5.f, 1.f);
	FireDesc.vAmColor = _float4(0.3f, 0.3f, 0.3f, 1.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Fire"), LEVEL_BOSS, TEXT("Layer_Fire"), &FireDesc)))
		return E_FAIL;


	FireDesc.vPos = _float3(-46.86f, 4.416f + 0.2f, -116.f);
	FireDesc.vScale = _float3(1.2f, 1.2f, 1.2f);
	FireDesc.fPower = CToolManager::Get_Instance()->Get_RendomNum(10.f, 15.f);
	FireDesc.fUpSpeed = CToolManager::Get_Instance()->Get_RendomNum(0.1f, 0.5f);
	FireDesc.fDownSpeed = FireDesc.fUpSpeed;
	FireDesc.vDiffuseColor = _float4(1.f, 1.f, 5.f, 1.f);
	FireDesc.vAmColor = _float4(0.3f, 0.3f, 0.3f, 1.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Fire"), LEVEL_BOSS, TEXT("Layer_Fire"), &FireDesc)))
		return E_FAIL;
	FireDesc.vPos = _float3(-44.91f, 2.637 + 0.2f, -111.6f);
	FireDesc.vScale = _float3(0.6f, 0.6f, 0.6f);
	FireDesc.fPower = CToolManager::Get_Instance()->Get_RendomNum(10.f, 15.f);
	FireDesc.fUpSpeed = CToolManager::Get_Instance()->Get_RendomNum(0.1f, 0.5f);
	FireDesc.fDownSpeed = FireDesc.fUpSpeed;
	FireDesc.vDiffuseColor = _float4(1.f, 1.f, 5.f, 1.f);
	FireDesc.vAmColor = _float4(0.3f, 0.3f, 0.3f, 1.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Fire"), LEVEL_BOSS, TEXT("Layer_Fire"), &FireDesc)))
		return E_FAIL;
	FireDesc.vPos = _float3(-62.35f, 3.613f+ 0.7f, -101.8f);
	FireDesc.vScale = _float3(1.5f, 1.5f, 1.5f);
	FireDesc.fPower = CToolManager::Get_Instance()->Get_RendomNum(10.f, 15.f);
	FireDesc.fUpSpeed = CToolManager::Get_Instance()->Get_RendomNum(0.1f, 0.5f);
	FireDesc.fDownSpeed = FireDesc.fUpSpeed;
	FireDesc.vDiffuseColor = _float4(1.f, 1.f, 5.f, 1.f);
	FireDesc.vAmColor = _float4(0.3f, 0.3f, 0.3f, 1.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Fire"), LEVEL_BOSS, TEXT("Layer_Fire"), &FireDesc)))
		return E_FAIL;
	FireDesc.vPos = _float3(-74.13f, 3.916f + 0.2f, -166.6f);
	FireDesc.vScale = _float3(1.3f, 1.3f, 1.3f);
	FireDesc.fPower = CToolManager::Get_Instance()->Get_RendomNum(10.f, 15.f);
	FireDesc.fUpSpeed = CToolManager::Get_Instance()->Get_RendomNum(0.1f, 0.5f);
	FireDesc.fDownSpeed = FireDesc.fUpSpeed;
	FireDesc.vDiffuseColor = _float4(1.f, 1.f, 5.f, 1.f);
	FireDesc.vAmColor = _float4(0.3f, 0.3f, 0.3f, 1.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Fire"), LEVEL_BOSS, TEXT("Layer_Fire"), &FireDesc)))
		return E_FAIL;
	FireDesc.vPos = _float3(-68.39f, 4.74f+ 0.2f, -126.7f);
	FireDesc.vScale = _float3(2.f, 2.f, 2.f);
	FireDesc.fPower = CToolManager::Get_Instance()->Get_RendomNum(10.f, 15.f);
	FireDesc.fUpSpeed = CToolManager::Get_Instance()->Get_RendomNum(0.1f, 0.5f);
	FireDesc.fDownSpeed = FireDesc.fUpSpeed;
	FireDesc.vDiffuseColor = _float4(1.f, 1.f, 5.f, 1.f);
	FireDesc.vAmColor = _float4(0.3f, 0.3f, 0.3f, 1.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Fire"), LEVEL_BOSS, TEXT("Layer_Fire"), &FireDesc)))
		return E_FAIL;
	FireDesc.vPos = _float3(-63.05f, 2.07f + 0.2f, -128.7f);
	FireDesc.vScale = _float3(0.3f, 0.3f, 0.3f);
	FireDesc.fPower = CToolManager::Get_Instance()->Get_RendomNum(10.f, 15.f);
	FireDesc.fUpSpeed = CToolManager::Get_Instance()->Get_RendomNum(0.1f, 0.5f);
	FireDesc.fDownSpeed = FireDesc.fUpSpeed;
	FireDesc.vDiffuseColor = _float4(1.f, 1.f, 5.f, 1.f);
	FireDesc.vAmColor = _float4(0.3f, 0.3f, 0.3f, 1.f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Fire"), LEVEL_BOSS, TEXT("Layer_Fire"), &FireDesc)))
		return E_FAIL;



	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_VSnatcher"), LEVEL_BOSS, TEXT("Layer_VSnatcher"))))
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

HRESULT CLevel_Boss::Ready_Layer_UI(const _tchar * pLayerTag)
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

HRESULT CLevel_Boss::Ready_Layer_NPC(const _tchar * pLayerTag)
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



CLevel_Boss * CLevel_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Boss*		pInstance = new CLevel_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Boss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Boss::Free()
{
	__super::Free();

}


