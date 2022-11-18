#include "stdafx.h"
#include "..\Public\Level_CamTool.h"

#include "GameInstance.h"
#include "CamManager.h"
#include "Camera_CamTool.h"
#include "ColorCube.h"

CLevel_CamTool::CLevel_CamTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_CamTool::Initialize()
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

	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	//	return E_FAIL;



	return S_OK;
}

void CLevel_CamTool::Tick(_float fTimeDelta)
{
	Get_Input(fTimeDelta);


	__super::Tick(fTimeDelta);
}

HRESULT CLevel_CamTool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	SetWindowText(g_hWnd, TEXT("Ä·Åø ·¹º§ÀÓ"));

	return S_OK;
}

void CLevel_CamTool::Get_Input(_float fTimeDelta)
{

	CCamManager::Get_Instance()->Tick(fTimeDelta);

}

HRESULT CLevel_CamTool::Ready_Lights()
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

HRESULT CLevel_CamTool::Ready_Layer_Camera(const _tchar * pLayerTag)
{

	CCamManager::Get_Instance()->Create_CamTool();


	return S_OK;
}

HRESULT CLevel_CamTool::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);






	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_CamTool::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamManager::Get_Instance()->Create_SelectingCube();

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Terrain"), LEVEL_CAMTOOL, pLayerTag)))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Sky"), LEVEL_CAMTOOL, pLayerTag)))
	//	return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_CamTool::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);




	Safe_Release(pGameInstance);


	return S_OK;
}

CLevel_CamTool * CLevel_CamTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_CamTool*		pInstance = new CLevel_CamTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_CamTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_CamTool::Free()
{
	__super::Free();

}


