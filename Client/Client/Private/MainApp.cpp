#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"

#include "DataManager.h"
#include "ToolManager.h"
#include "ItemManager.h"
#include "CamManager.h"
#include "UIManager.h"
#include "GameManager.h"
#include "CutSceneManager.h"

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



	if (FAILED(CDataManager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CToolManager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CItemManager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CCamManager::Get_Instance()->Init()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Fonts(m_pDevice, m_pContext, TEXT("Font_Nexon"), TEXT("../Bin/Resources/Fonts/129ex.spritefont"))))
		return E_FAIL;


	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;
	
	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return ;

#ifdef _DEBUG

	if (m_pGameInstance->Key_Down(DIK_F11))
	{
		if (CToolManager::Get_Instance()->Get_Debug())
			CToolManager::Get_Instance()->Set_Debug(false);
		else
			CToolManager::Get_Instance()->Set_Debug(true);
	}
		

	m_fTimeAcc += fTimeDelta;
#endif // _DEBUG

	m_pGameInstance->Tick_Engine(fTimeDelta);
	


	m_pGameInstance->Calcul_ColGroup(CColliderManager::COLLIDER_PLAYER, CColliderManager::COLLIDER_MONSTER);
	m_pGameInstance->Calcul_ColGroup(CColliderManager::COLLIDER_PLAYER, CColliderManager::COLLIDER_PUSHMONSTER);
	m_pGameInstance->Calcul_ColGroup(CColliderManager::COLLIDER_PLAYER, CColliderManager::COLLIDER_NPC);
	m_pGameInstance->Calcul_ColGroup(CColliderManager::COLLIDER_PLAYER, CColliderManager::COLLIDER_EM);
	m_pGameInstance->Calcul_ColGroup(CColliderManager::COLLIDER_PLAYER, CColliderManager::COLLIDER_ITEM);
	m_pGameInstance->Calcul_ColGroup(CColliderManager::COLLIDER_PUSHMONSTER, CColliderManager::COLLIDER_PUSHMONSTER);
	m_pGameInstance->Calcul_ColGroup(CColliderManager::COLLIDER_SWORD, CColliderManager::COLLIDER_MONSTER);
	m_pGameInstance->Calcul_ColGroup(CColliderManager::COLLIDER_SWORD, CColliderManager::COLLIDER_PUSHMONSTER);
	m_pGameInstance->Calcul_ColGroup(CColliderManager::COLLIDER_SWORD, CColliderManager::COLLIDER_NPC);
	m_pGameInstance->Clear_ColGroup();


	CGameManager::Get_Instance()->Tick(fTimeDelta);



	if (m_pGameInstance->Key_Down(DIK_TAB))
	{
		if (m_bTap)
			m_bTap = false;
		else
			m_bTap = true;
	}

	if (m_bTap)
	{
		RECT Clip;
		GetClientRect(g_hWnd, &Clip);
		ClientToScreen(g_hWnd, (LPPOINT)&Clip);
		ClientToScreen(g_hWnd, (LPPOINT)(&Clip.right));
		ClipCursor(&Clip);
	}
	else
		ClipCursor(nullptr);

}

HRESULT CMainApp::Render()
{

	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();
	
	m_pRenderer->Draw();
	m_pGameInstance->Render_Level();
	CGameManager::Get_Instance()->Clear_Data();

	m_pGameInstance->Present();

#ifdef _DEBUG
	++m_iNumDraw;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("fps : %d"), m_iNumDraw);
		m_iNumDraw = 0;
		m_fTimeAcc = 0.f;
	}

	 SetWindowText(g_hWnd, m_szFPS);
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
	Safe_AddRef(m_pRenderer);

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
	//	return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Sockat */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sockat"),
		CSockat::Create(m_pDevice, m_pContext))))
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

	CCutSceneManager::Get_Instance()->Destroy_Instance();
	CGameManager::Get_Instance()->Destroy_Instance();
	CUIManager::Get_Instance()->Destroy_Instance();
	CToolManager::Get_Instance()->Destroy_Instance();
	CItemManager::Get_Instance()->Destroy_Instance();
	CCamManager::Get_Instance()->Destroy_Instance();
	CDataManager::Get_Instance()->Destroy_Instance();


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();

}
