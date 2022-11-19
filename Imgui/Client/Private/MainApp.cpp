#include "stdafx.h"
#include "..\Public\MainApp.h"
#include "GameInstance.h"

#include "ImGui_Manager.h"
#include "ToolManager.h"
#include "CamManager.h"
#include "MapManager.h"
#include "DataManager.h"
#include "AnimManager.h"
#include "PartsManager.h"
#include "MeshManager.h"
#include "ParticleManager.h"

#include "Level_Loading.h"

using namespace Client;

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	GRAPHICDESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWinMode = GRAPHICDESC::MODE_WIN;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, g_hInst, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;


	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(CParticleManager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CMapManager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CDataManager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CToolManager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CImGui_Manager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CAnimManager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CPartsManager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CMeshManager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(CToolManager::Get_Instance()->Change_Level(LEVEL_GAMEPLAY)))
		return E_FAIL;
	
	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return ;

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif // _DEBUG

	m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{

	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.1f, 0.1f, 0.1f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw();

	m_pGameInstance->Render_Level();
	CImGui_Manager::Get_Instance()->Tick(0.f);
	CImGui_Manager::Get_Instance()->Render();


	m_pGameInstance->Present();

#ifdef _DEBUG
	//++m_iNumDraw;

	//if (m_fTimeAcc >= 1.f)
	//{
	//	wsprintf(m_szFPS, TEXT("fps : %d"), m_iNumDraw);
	//	m_iNumDraw = 0;
	//	m_fTimeAcc = 0.f;
	//}

	//SetWindowText(g_hWnd, m_szFPS);
#endif // _DEBUG

	

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* 특정 레벨을 할당하기 전에 로딩레벨을 거쳐 할당되는 형태로 가자. */

	/* 할당한 레벨을 레벨매니져에 보관할 수 있또록. gksek. */
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}



HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Sockat */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sockat"),
		CSockat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Sockat */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CAABB::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		COBB::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CSphere::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;
	

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{

	return S_OK;
}

CMainApp * CMainApp::Create()
{	
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CMainApp"));
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CMainApp::Free()
{

	CParticleManager::Destroy_Instance();
	CMeshManager::Destroy_Instance();
	CPartsManager::Destroy_Instance();
	CAnimManager::Destroy_Instance();
	CDataManager::Destroy_Instance();
	CToolManager::Destroy_Instance();
	CCamManager::Destroy_Instance();
	CMapManager::Destroy_Instance();
	CImGui_Manager::Destroy_Instance();


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine();

}
