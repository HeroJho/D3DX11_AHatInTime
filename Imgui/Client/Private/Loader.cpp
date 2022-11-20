#include "stdafx.h"
#include "..\Public\Loader.h"

#include "DataManager.h"
#include "ToolManager.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_CamTool.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Terrain_Map.h"
#include "Terrain_Anim.h"
#include "Monster.h"
#include "Player.h"
#include "AnimPlayer.h"
#include "Sky.h"
#include "VIBuffer_ColorCube.h"
#include "ColorCube.h"
#include "MarkCube.h"
#include "LookCube.h"
#include "CamSelectingCube.h"
#include "RenderCube.h"
#include "StaticModel.h"
#include "AnimModel.h"
#include "Ori_Hat.h"
#include "Parts.h"
#include "ClickedVertexCube.h"
#include "SkyModel.h"
#include "Particle.h"


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
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	case LEVEL_SELECTTOOL:
		pLoader->Loading_ForSelectToolLevel();
		break;
	case LEVEL_MAPTOOL:
		pLoader->Loading_ForMapToolLevel();
		break;
	case LEVEL_CAMTOOL:
		pLoader->Loading_ForCamToolLevel();
		break;
	case LEVEL_ANIMTOOL :
		pLoader->Loading_ForAnimToolLevel();
		break;
	case LEVEL_PARTSTOOL :
		pLoader->Loading_ForPartsToolLevel();
		break;
	case LEVEL_TESTLEVEL:
		pLoader->Loading_ForTestLevel();
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
	/* For.Prototype_GameObject_Terrain_Map*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Map"),
		CTerrain_Map::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Terrain_Anim*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Anim"),
		CTerrain_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_AnimPlayer*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AnimPlayer"),
		CAnimPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_CamTool */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_CamTool"),
		CCamera_CamTool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ColorCube */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ColorCube"),
		CColorCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_MarkCube */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MarkCube"),
		CColorCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_LookCube */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LookCube"),
		CColorCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_CamSelectingCube */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CamSelectingCube"),
		CCamSelectingCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ColorCube */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RenderCube"),
		CRenderCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ClickedVertexCube */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ClickedVertexCube"),
		CClickedVertexCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	/* For.Prototype_GameObject_Terrain_StaticModel*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticModel"),
		CStaticModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain_AnimModel*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AnimModel"),
		CAnimModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Ori_Hat*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ori_Hat"),
		COri_Hat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Parts*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Parts"),
		CParts::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Sky*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSkyModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Particle*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle"),
		CParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;







	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_T_FX_Flare_01 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_FX_Flare_01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/T_FX_Flare_01.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Star */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/Star_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_star_shuriken */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_star_shuriken"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/star_shuriken.dds"), 1))))
		return E_FAIL;







	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* 모델를 로드한다. */

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_Map_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Map_Terrain"),
		CVIBuffer_Map_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_ColorCube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_ColorCube"),
		CVIBuffer_ColorCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	// TEST
	/* For.Prototype_Component_Sky */
	_matrix mPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Sky"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/SubconSky/", "SubconSky.fbx", mPivot))))
		return E_FAIL;


	if (FAILED(Loading_Model_NoneAnim()))
		return E_FAIL;




	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));
	/* 셰이더를 로드한다. */

	/* For.Prototype_Component_Shader_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Terrain"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_ColorCube*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_ColorCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Vtx.hlsl"), VTX_DECLARATION::Elements, VTX_DECLARATION::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_Model */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_AnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;






	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForSelectToolLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);






	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));
	/* 객체원형 로드한다. */






	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */







	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* 모델를 로드한다. */





	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForMapToolLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);






	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));
	/* 객체원형 로드한다. */






	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */







	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* 모델를 로드한다. */










	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForCamToolLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);






	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));
	/* 객체원형 로드한다. */






	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */







	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* 모델를 로드한다. */





	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForAnimToolLevel()
{

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);






	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));
	/* 객체원형 로드한다. */






	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */







	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* 모델를 로드한다. */





	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForPartsToolLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);






	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));
	/* 객체원형 로드한다. */






	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */







	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* 모델를 로드한다. */





	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForTestLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);






	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));
	/* 객체원형 로드한다. */






	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */







	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* 모델를 로드한다. */





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

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);


}
