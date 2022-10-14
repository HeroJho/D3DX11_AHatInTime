#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "DataManager.h"

#include "Camera_Free.h"
#include "Terrain.h"
#include "Monster.h"
#include "Player.h"
#include "UI_Edit.h"
#include "UI_Edit_Button.h"
#include "StaticModel.h"
#include "Ori_Hat.h"
#include "Umbrella.h"
#include "UI_Health.h"
#include "UI_TextLife.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CS());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CS());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));
	/* 객체원형 로드한다. */	

	// TEST
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Edit"),
		CUI_Edit::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Edit_Button */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Edit_Button"),
		CUI_Edit_Button::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */


	/* For.Prototype_Component_Texture_Default */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	// TEST
	/* For.Prototype_Component_Texture_InvenFrame */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/TestUI/Inventory/InvenFrame.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_InvenBtn */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenBtn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/TestUI/Inventory/InvenBtn/CashBtn%d.png"), 2))))
		return E_FAIL;







	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));	
	/* 모델를 로드한다. */







	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));

	/* 개ㅑㄱ체원형 로드한다. */
	/* For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), 
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain_StaticModel*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticModel"),
		CStaticModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Ori_Hat*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ori_Hat"),
		COri_Hat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Umbrella*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Umbrella"),
		CUmbrella::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	/* For.Prototype_UI_Health*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Health"),
		CUI_Health::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_UI_TextLife*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_TextLife"),
		CUI_TextLife::Create(m_pDevice, m_pContext))))
		return E_FAIL;








	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;



	// ==================== UI=====================
	/* For.Prototype_Component_Texture_PlayerHealth */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerHealth"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerHealth/playerhealth_new_%d.dds"), 5))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_LifeText */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LifeText"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerHealth/text_life.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_SmartEye */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SmartEye"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Meshes/Anim/HatGirl/hatkid_smarteyes.dds"), 1))))
		return E_FAIL;





	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* 모델를 로드한다. */

	/* For.Prototype_Component_VIBuffer_Terrain */

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Map_Terrain"),
		CVIBuffer_Map_Terrain::Create(m_pDevice, m_pContext, 100, 100))))
		return E_FAIL;



	if (FAILED(Loading_Model_NoneAnim()))
		return E_FAIL;


	/* For. AnimModel */
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("HatGirl"), LEVEL_STATIC, CDataManager::DATA_ANIM);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("Mad_Crow"), LEVEL_GAMEPLAY, CDataManager::DATA_ANIM);

	/* For. PartsModel */
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("Ori_Hat"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("Umbrella"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);



	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));

	/* For.Prototype_Component_Shader_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Terrain"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Model */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;




	lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));

	vector<CCell*> Cells = CDataManager::Get_Instance()->Load_Navi(0);

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, &Cells))))
		return E_FAIL;




	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}


HRESULT CLoader::Loading_Model_NoneAnim()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



	const list<string>* FileNames = CDataManager::Get_Instance()->Get_NonAnimFileNames();

	_matrix		PivotMatrix = XMMatrixIdentity();
	for (auto& sFileName : *FileNames)
	{
		_tchar czTag[MAX_PATH];
		_tchar* cTag = CToolManager::Get_Instance()->Get_ManagedTChar();
		CToolManager::Get_Instance()->CtoTC(sFileName.data(), czTag);

		memcpy(cTag, czTag, sizeof(_tchar) * MAX_PATH);

		CDataManager::Get_Instance()->Create_Try_BinModel(cTag, LEVEL_GAMEPLAY, CDataManager::DATA_NOEANIM);
	}




	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	CloseHandle(m_hThread);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);


}
