#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"

#include "DataManager.h"
#include "ItemManager.h"

#include "Camera_Free.h"
#include "UI.h"


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

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;


	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fTimeAcc += fTimeDelta;

	if (10.f < m_fTimeAcc)
	{
		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		
		CGameObject::CREATUREINFODESC ObjDesc;
		ZeroMemory(&ObjDesc, sizeof(CGameObject::CREATUREINFODESC));
		ObjDesc.iAT = 1;
		ObjDesc.iMaxHP = 3;
		ObjDesc.iHP = 3;

		pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Monster"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &ObjDesc);


		Safe_Release(pGameInstance);

		m_fTimeAcc = 0.f;
	}


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
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 0.f, 0.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamera::CAMERADESC			CameraDesc;

	CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.0f;

	CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Camera_Free"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

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
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Terrain"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_RollingBarrel"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_RectBarrel"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;
	

	if (FAILED(CDataManager::Get_Instance()->Load_Map(0, LEVEL_GAMEPLAY)))
		return E_FAIL;




	if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("yarn_ui_sprint"), LEVEL_GAMEPLAY, _float3(-40.75f, 15.34f, 157.85f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f))))
		return E_FAIL;
	if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("yarn_ui_ice"), LEVEL_GAMEPLAY, _float3(-41.75f, 15.34f, 157.85f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f))))
		return E_FAIL;
	if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("yarn_ui_hover"), LEVEL_GAMEPLAY, _float3(-42.75f, 15.34f, 157.85f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f))))
		return E_FAIL;
	if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("yarn_ui_brew"), LEVEL_GAMEPLAY, _float3(-43.75f, 15.34f, 157.85f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f))))
		return E_FAIL;
	if (FAILED(CItemManager::Get_Instance()->Make_Item(TEXT("Umbrella"), LEVEL_GAMEPLAY, _float3(-44.75f, 15.34f, 157.85f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f))))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{


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


