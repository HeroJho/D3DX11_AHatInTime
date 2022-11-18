#include "stdafx.h"
#include "..\Public\Level_MapTool.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "MeshManager.h"
#include "MapManager.h"
#include "CamManager.h"

CLevel_MapTool::CLevel_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_MapTool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;


	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;



	CCamManager::Get_Instance()->Create_CamTool();
	CCamManager::Get_Instance()->Create_SelectingCube();



	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//LIGHTDESC			LightDesc;
	//ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	//LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	//LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	//LightDesc.vDiffuse = _float4(0.1f, 0.1f, 0.1f, 1.f);
	//LightDesc.vAmbient = _float4(.2f, .2f, .2f, 1.f);
	//LightDesc.vSpecular = _float4(139.f / 255.f, 0.f / 255.f, 255.f / 255.f, 1.f);

	//if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc, 0)))
	//	return E_FAIL;


	//ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	//LightDesc.eType = LIGHTDESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(-68.97f, 13.04f, 121.20f, 1.f);
	//LightDesc.fRange = 30.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CLevel_MapTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if(CMapManager::Get_Instance()->Get_NaviMode())
		CMeshManager::Get_Instance()->Tick(fTimeDelta);
	else
		CMapManager::Get_Instance()->Tick(fTimeDelta);

	RELEASE_INSTANCE(CGameInstance);


	CCamManager::Get_Instance()->Tick(fTimeDelta);

}

HRESULT CLevel_MapTool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("¸ÊÅø ·¹º§ÀÓ"));

	if (CMapManager::Get_Instance()->Get_NaviMode())
	{
		CMeshManager::Get_Instance()->Comput_Cell();
		CMeshManager::Get_Instance()->Comput_FreeVectexCube();
	}
	else
	{
		CMapManager::Get_Instance()->Cul_MinClickedModel();
	}

	CMeshManager::Get_Instance()->Render();

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_Camera(const _tchar * pLayerTag)
{


	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, pLayerTag)))
	//	return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Sky"), LEVEL_MAPTOOL, pLayerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);






	Safe_Release(pGameInstance);


	return S_OK;
}

CLevel_MapTool * CLevel_MapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_MapTool*		pInstance = new CLevel_MapTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_MapPlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_MapTool::Free()
{
	__super::Free();

}


