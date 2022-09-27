#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "GameInstance.h"
#include "ImGui_Manager.h"

#include "Level_GamePlay.h"
#include "Level_SelectTool.h"
#include "Level_MapTool.h"
#include "Level_CamTool.h"
#include "Level_AnimTool.h"
#include "Level_TestLevel.h"

#include "Loader.h"


CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pDeviceContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (true == m_pLoader->Get_Finished())
	{
		
			CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			CLevel*		pNewLevel = nullptr;



			switch (m_eNextLevel)
			{
			case LEVEL_GAMEPLAY:
				pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pDeviceContext);
				break;
			case LEVEL_SELECTTOOL:
				pNewLevel = CLevel_SelectTool::Create(m_pDevice, m_pDeviceContext);
				break;
			case LEVEL_MAPTOOL:
				pNewLevel = CLevel_MapTool::Create(m_pDevice, m_pDeviceContext);
				break;
			case LEVEL_CAMTOOL:
				pNewLevel = CLevel_CamTool::Create(m_pDevice, m_pDeviceContext);
				break;
			case LEVEL_ANIMTOOL:
				pNewLevel = CLevel_AnimTool::Create(m_pDevice, m_pDeviceContext);
				break;
			case LEVEL_TESTLEVEL:
				pNewLevel = CLevel_TestLevel::Create(m_pDevice, m_pDeviceContext);
				break;
			}



			if (nullptr == pNewLevel)
				goto except;

			CImGui_Manager::Get_Instance()->Init_Level(m_eNextLevel);
			if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pNewLevel)))
				goto except;

	
			except:
				Safe_Release(pGameInstance);
	}
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;

}


void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);

}


