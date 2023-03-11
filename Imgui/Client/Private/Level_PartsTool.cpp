#include "stdafx.h"
#include "..\Public\LEVEL_PartsTool.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "ToolManager.h"
#include "PartsManager.h"

CLevel_PartsTool::CLevel_PartsTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_PartsTool::Initialize()
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

	// TEST
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;



	return S_OK;
}

void CLevel_PartsTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


}

HRESULT CLevel_PartsTool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	SetWindowText(g_hWnd, TEXT("게임플레이레벨임"));

	return S_OK;
}

HRESULT CLevel_PartsTool::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc, 0)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_PartsTool::Ready_Layer_Camera(const _tchar * pLayerTag)
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

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Camera_Free"), LEVEL_PARTSTOOL, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_PartsTool::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);





	Safe_Release(pGameInstance);





	CPartsManager::Get_Instance()->Create_Player();


	

	return S_OK;
}

HRESULT CLevel_PartsTool::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);




	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Terrain_Anim"), LEVEL_PARTSTOOL, pLayerTag)))
		return E_FAIL;



	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_PartsTool::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	//if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Terrain_Anim"), LEVEL_PARTSTOOL, pLayerTag)))
	//	return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}


CLevel_PartsTool * CLevel_PartsTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_PartsTool*		pInstance = new CLevel_PartsTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_PartsTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_PartsTool::Free()
{
	__super::Free();

}


