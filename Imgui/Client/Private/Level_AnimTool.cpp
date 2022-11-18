#include "stdafx.h"
#include "..\Public\Level_AnimTool.h"

#include "GameInstance.h"
#include "CamManager.h"
#include "AnimManager.h"
#include "Camera_CamTool.h"
#include "ColorCube.h"
#include "Camera_Free.h"

CLevel_AnimTool::CLevel_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_AnimTool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	CCamManager::Get_Instance()->Create_CamTool();
	CCamManager::Get_Instance()->Create_SelectingCube();

	//if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	//	return E_FAIL;



	return S_OK;
}

void CLevel_AnimTool::Tick(_float fTimeDelta)
{
	Get_Input(fTimeDelta);

	CAnimManager::Get_Instance()->Tick(fTimeDelta);

	__super::Tick(fTimeDelta);
}

HRESULT CLevel_AnimTool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	SetWindowText(g_hWnd, TEXT("애니툴 레벨임"));

	return S_OK;
}

void CLevel_AnimTool::Get_Input(_float fTimeDelta)
{

	CCamManager::Get_Instance()->Tick(fTimeDelta);

}

HRESULT CLevel_AnimTool::Ready_Lights()
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

HRESULT CLevel_AnimTool::Ready_Layer_Camera(const _tchar * pLayerTag)
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

	CameraDesc.TransformDesc.fSpeedPerSec = 20.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Camera_Free"), LEVEL_ANIMTOOL, pLayerTag, &pObj, &CameraDesc)))
		return E_FAIL;

	CAnimManager::Get_Instance()->Set_Cam((CCamera_Free*)pObj);

	Safe_Release(pGameInstance);


	return S_OK;
}

HRESULT CLevel_AnimTool::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);






	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_AnimTool::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);



	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Terrain_Anim"), LEVEL_ANIMTOOL, pLayerTag)))
		return E_FAIL;



	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_AnimTool::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);






	Safe_Release(pGameInstance);


	return S_OK;
}

CLevel_AnimTool * CLevel_AnimTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_AnimTool*		pInstance = new CLevel_AnimTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_AnimTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_AnimTool::Free()
{
	__super::Free();

}


