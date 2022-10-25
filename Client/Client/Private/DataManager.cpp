#include "stdafx.h"
#include "..\Public\DataManager.h"

#include "GameInstance.h"

#include "ToolManager.h"

#include "StaticModel_Col.h"
#include "StaticModel_Instance.h"
#include "Cell.h"


IMPLEMENT_SINGLETON(CDataManager)

CDataManager::CDataManager()
{
}

HRESULT CDataManager::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	if (FAILED(LoadModelPass()))
		return E_FAIL;

	return S_OK;
}



HRESULT CDataManager::LoadModelPass()
{
	// ../Bin/Resources/Meshes/Anim/모델명
	// ../Bin/Resources/Meshes/NoneAnim/모델명

	// 경로를 다 읽어온다.
	WIN32_FIND_DATA data;

	HANDLE hFind = FindFirstFile(TEXT("../Bin/Resources/Meshes/NonAnim//*"), &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			// TC -> C
			char temp[MAX_PATH];
			CToolManager::Get_Instance()->TCtoC(data.cFileName, temp);

			m_NonAnimFilePaths.push_back(temp);

		} while (FindNextFile(hFind, &data) != 0);

		FindClose(hFind);
	}

	m_NonAnimFilePaths.pop_front();
	m_NonAnimFilePaths.pop_front();





	hFind = FindFirstFile(TEXT("../Bin/Resources/Meshes/Anim//*"), &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			// TC -> C
			char temp[MAX_PATH];
			CToolManager::Get_Instance()->TCtoC(data.cFileName, temp);

			m_AnimFilePaths.push_back(temp);

		} while (FindNextFile(hFind, &data) != 0);

		FindClose(hFind);
	}

	m_AnimFilePaths.pop_front();
	m_AnimFilePaths.pop_front();




	hFind = FindFirstFile(TEXT("../Bin/Resources/Meshes/Parts//*"), &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			// TC -> C
			char temp[MAX_PATH];
			CToolManager::Get_Instance()->TCtoC(data.cFileName, temp);

			m_PartFilePaths.push_back(temp);

		} while (FindNextFile(hFind, &data) != 0);

		FindClose(hFind);
	}

	m_PartFilePaths.pop_front();
	m_PartFilePaths.pop_front();

	return S_OK;
}


HRESULT CDataManager::ReadSceneData(char * pFileName, DATA_HEROSCENE* ReadScene, DATA_TYPE eTYPE)
{
	char cPullName[MAX_PATH];
	if (DATA_ANIM == eTYPE)
		strcpy_s(cPullName, "../Bin/ToolData/Bin_Model/Anim/");
	else if (DATA_NOEANIM == eTYPE)
		strcpy_s(cPullName, "../Bin/ToolData/Bin_Model/NonAnim/");
	else if (DATA_PARTS == eTYPE)
		strcpy_s(cPullName, "../Bin/ToolData/Bin_Model/Parts/");

	strcat_s(cPullName, pFileName);


	ZeroMemory(ReadScene, sizeof(DATA_HEROSCENE));
	std::ifstream ifs(cPullName, ios::in | ios::binary);

	if (!ifs)
		return E_FAIL;

	// Node
	int iNodeCount = 0;
	ifs.read((char*)&iNodeCount, sizeof(int));
	ReadScene->iNodeCount = iNodeCount;
	ReadScene->pHeroNodes = new DATA_HERONODE[iNodeCount];
	for (int i = 0; i < iNodeCount; ++i)
	{
		DATA_HERONODE* pHeroNode = &ReadScene->pHeroNodes[i];
		ifs.read((char*)pHeroNode, sizeof(DATA_HERONODE));
	}

	// Material
	int iMaterialCount = 0;
	ifs.read((char*)&iMaterialCount, sizeof(int));
	ReadScene->iMaterialCount = iMaterialCount;
	ReadScene->pHeroMaterial = new DATA_HEROMATERIAL[iMaterialCount];
	for (int i = 0; i < iMaterialCount; ++i)
	{
		DATA_HEROMATERIAL* pHeroMarterial = &ReadScene->pHeroMaterial[i];
		ifs.read((char*)pHeroMarterial, sizeof(DATA_HEROMATERIAL));
	}

	// MashNode
	int iMashCount = 0;
	ifs.read((char*)&iMashCount, sizeof(int));
	ReadScene->iMeshCount = iMashCount;
	ReadScene->pHeroMesh = new DATA_HEROMETH[iMashCount];
	for (int i = 0; i < iMashCount; ++i)
	{
		DATA_HEROMETH* pHeroMash = &ReadScene->pHeroMesh[i];

		ifs.read((char*)&pHeroMash->cName, sizeof(char)*MAX_PATH);
		ifs.read((char*)&pHeroMash->iMaterialIndex, sizeof(int));

		ifs.read((char*)&pHeroMash->NumVertices, sizeof(int));

		int iIsAnim = 0;
		ifs.read((char*)&iIsAnim, sizeof(int));
		if (iIsAnim)
		{
			pHeroMash->pAnimVertices = new VTXANIMMODEL[pHeroMash->NumVertices];
			pHeroMash->pNonAnimVertices = nullptr;
			for (int j = 0; j < pHeroMash->NumVertices; ++j)
			{
				VTXANIMMODEL* VtxAniModel = &pHeroMash->pAnimVertices[j];
				ifs.read((char*)VtxAniModel, sizeof(VTXANIMMODEL));
			}
		}
		else
		{
			pHeroMash->pNonAnimVertices = new VTXMODEL[pHeroMash->NumVertices];
			pHeroMash->pAnimVertices = nullptr;
			for (int j = 0; j < pHeroMash->NumVertices; ++j)
			{
				VTXMODEL* VtxNonAniModel = &pHeroMash->pNonAnimVertices[j];
				ifs.read((char*)VtxNonAniModel, sizeof(VTXMODEL));
			}
		}

		pHeroMash->iNumPrimitives = 0;
		ifs.read((char*)&pHeroMash->iNumPrimitives, sizeof(int));
		pHeroMash->pIndices = new FACEINDICES32[pHeroMash->iNumPrimitives];
		for (int j = 0; j < pHeroMash->iNumPrimitives; ++j)
		{
			FACEINDICES32* Indices32 = &pHeroMash->pIndices[j];
			ifs.read((char*)Indices32, sizeof(FACEINDICES32));
		}


		pHeroMash->iNumBones = 0;
		ifs.read((char*)&pHeroMash->iNumBones, sizeof(int));
		pHeroMash->pBones = new DATA_HEROBONE[pHeroMash->iNumBones];
		for (int j = 0; j < pHeroMash->iNumBones; ++j)
		{
			DATA_HEROBONE* bon = &pHeroMash->pBones[j];
			ifs.read((char*)bon, sizeof(DATA_HEROBONE));
		}
	}


	// Animation
	int iNumAnimations = 0;
	ifs.read((char*)&iNumAnimations, sizeof(int));
	ReadScene->iNumAnimations = iNumAnimations;
	ReadScene->pHeroAnim = new DATA_HEROANIM[iNumAnimations];

	for (int i = 0; i < iNumAnimations; ++i)
	{
		ifs.read((char*)&ReadScene->pHeroAnim[i].iNumChannels, sizeof(int));
		ifs.read((char*)&ReadScene->pHeroAnim[i].fDuration, sizeof(float));
		ifs.read((char*)&ReadScene->pHeroAnim[i].fTickPerSecond, sizeof(float));
		ifs.read((char*)&ReadScene->pHeroAnim[i].bLoop, sizeof(bool));
		ifs.read((char*)&ReadScene->pHeroAnim[i].szName, sizeof(char) * MAX_PATH);

		ReadScene->pHeroAnim[i].pHeroChannel = new DATA_HEROCHANNEL[ReadScene->pHeroAnim[i].iNumChannels];
		for (int j = 0; j < ReadScene->pHeroAnim[i].iNumChannels; ++j)
		{
			DATA_HEROCHANNEL* pChannel = &ReadScene->pHeroAnim[i].pHeroChannel[j];
			ifs.read((char*)&pChannel->szName, sizeof(char)*MAX_PATH);
			ifs.read((char*)&pChannel->iNumKeyFrames, sizeof(int));

			pChannel->pKeyFrames = new KEYFRAME[pChannel->iNumKeyFrames];
			for (int k = 0; k < pChannel->iNumKeyFrames; ++k)
			{
				KEYFRAME* pKeyFrame = &pChannel->pKeyFrames[k];
				ifs.read((char*)pKeyFrame, sizeof(KEYFRAME));
			}
		}
	}


	ifs.close();
	return S_OK;
}


HRESULT CDataManager::Create_Try_BinModel(const _tchar * pModelName, LEVEL eLEVEL, DATA_TYPE eTYPE)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	// 원본 체크
	if (FAILED(pGameInstance->Check_Prototype(eLEVEL, pModelName)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}
	RELEASE_INSTANCE(CGameInstance);



	char cTempName[MAX_PATH];
	CToolManager::Get_Instance()->TCtoC(pModelName, cTempName);

	char* tPath = CToolManager::Get_Instance()->Get_ManagedChar();
	if (DATA_ANIM == eTYPE)
		strcpy(tPath, "../Bin/Resources/Meshes/Anim/");
	else if (DATA_NOEANIM == eTYPE)
		strcpy(tPath, "../Bin/Resources/Meshes/NonAnim/");
	else if (DATA_PARTS == eTYPE)
		strcpy(tPath, "../Bin/Resources/Meshes/Parts/");

	strcat(tPath, cTempName);
	strcat(tPath, "/");

	char* tFileName = CToolManager::Get_Instance()->Get_ManagedChar();
	strcpy(tFileName, cTempName);
	char TempName[MAX_PATH];
	strcpy(TempName, tFileName);
	strcat(tFileName, ".fbx");

	// Bin 체크 And Load
	DATA_HEROSCENE* Scene = new DATA_HEROSCENE;
	ZeroMemory(Scene, sizeof(DATA_HEROSCENE));
	_bool bIsBin = true;
	if (FAILED(CDataManager::Get_Instance()->ReadSceneData(TempName, Scene, eTYPE)))
	{
		bIsBin = false;
		Safe_Delete(Scene);
	}



	pGameInstance = GET_INSTANCE(CGameInstance);


	_matrix PivotMatrix;
	CModel::TYPE etype = CModel::TYPE_END;
	if (DATA_ANIM == eTYPE)
	{
		PivotMatrix = XMMatrixScaling(0.01f, 0.01, 0.01) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		etype = CModel::TYPE_ANIM;
	}
	else
	{
		// PivotMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		PivotMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
		etype = CModel::TYPE_NONANIM;
	}

	// 원본 생성
	if (bIsBin)
	{
		if (FAILED(pGameInstance->Add_Prototype(eLEVEL, pModelName,
			CModel::Bin_Create(m_pDevice, m_pContext, Scene, etype, tPath, tFileName, PivotMatrix))))
		{
			// 뭔가 파일 경로가 잘 못 됨.
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else
	{

		if (FAILED(pGameInstance->Add_Prototype(eLEVEL, pModelName,
			CModel::Create(m_pDevice, m_pContext, etype, tPath, tFileName, PivotMatrix))))
		{
			// 뭔가 파일 경로가 잘 못 됨.
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}









HRESULT CDataManager::Load_Map(_int iMapID, LEVEL eLEVEL)
{
	char cPullName[MAX_PATH];
	char cName[MAX_PATH];
	string ID = to_string(iMapID);

	strcpy_s(cName, "MAP_");
	strcat_s(cName, ID.data());
	strcpy_s(cPullName, "../Bin/ToolData/Map/");
	strcat_s(cPullName, cName);

	std::ifstream ifs(cPullName, ios::in | ios::binary);

	if (!ifs)
		return E_FAIL;

	DATA_MAP*  pData_Map = new DATA_MAP;
	ZeroMemory(pData_Map, sizeof(DATA_MAP));


	ifs.read((char*)&pData_Map->iID, sizeof(int));
	ifs.read((char*)&pData_Map->iNumObj, sizeof(int));

	pData_Map->pObjDatas = new DATA_MAP_OBJ[pData_Map->iNumObj];
	for (_int i = 0; i < pData_Map->iNumObj; ++i)
	{
		DATA_MAP_OBJ* DMJ = &pData_Map->pObjDatas[i];

		ifs.read((char*)DMJ->cName, sizeof(char)*MAX_PATH);
		ifs.read((char*)&DMJ->vPos, sizeof(_float3));
		ifs.read((char*)&DMJ->vAngle, sizeof(_float3));
		ifs.read((char*)&DMJ->vScale, sizeof(_float3));

		ifs.read((char*)&DMJ->vCenter, sizeof(_float3));
		ifs.read((char*)&DMJ->vRotation, sizeof(_float3));
		ifs.read((char*)&DMJ->vSize, sizeof(_float3));
		ifs.read((char*)&DMJ->bWall, sizeof(_bool));
	}

	ifs.close();








	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	map<string, list<_float4x4>> TempMap;

	for (_int i = 0; i < pData_Map->iNumObj; ++i)
	{
		CDataManager::DATA_MAP_OBJ DataObj = pData_Map->pObjDatas[i];


		CStaticModel_Col::STATICMODELDESC Desc;
		ZeroMemory(&Desc, sizeof(CStaticModel_Col::STATICMODELDESC));

		string sTemp = DataObj.cName;


		CToolManager::Get_Instance()->CtoTC(sTemp.data(), Desc.cModelTag);
		Desc.vPos = DataObj.vPos;
		Desc.vAngle = DataObj.vAngle;
		Desc.vScale = DataObj.vScale;

		Desc.vCenter = DataObj.vCenter;
		Desc.vRotation = DataObj.vRotation;
		Desc.vSize = DataObj.vSize;
		Desc.bWall = DataObj.bWall;

		// 콜라이더
		if (pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StaticModel"), eLEVEL, TEXT("Layer_Model"), &Desc))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}



		// Instancing
		auto& iter = TempMap.find(sTemp);
		if (iter == TempMap.end()) // 태그가 없다면 
		{
			list<_float4x4> flist;

			_matrix mScale = XMMatrixScaling(DataObj.vScale.x, DataObj.vScale.y, DataObj.vScale.z);
			_matrix mAngle = XMMatrixRotationX(XMConvertToRadians(DataObj.vAngle.x)) * XMMatrixRotationY(XMConvertToRadians(DataObj.vAngle.y)) * XMMatrixRotationZ(XMConvertToRadians(DataObj.vAngle.z));
			//_matrix mAngle = XMMatrixRotationX(DataObj.vAngle.x) * XMMatrixRotationY(DataObj.vAngle.y) * XMMatrixRotationZ(DataObj.vAngle.z);
			_matrix mPos = XMMatrixTranslation(DataObj.vPos.x, DataObj.vPos.y, DataObj.vPos.z);

			_matrix mInstanceLocal = mScale * mAngle * mPos;
			_float4x4 mStoreLocal;
			XMStoreFloat4x4(&mStoreLocal, mInstanceLocal);

			flist.push_back(mStoreLocal);


			TempMap.insert({ sTemp, flist });
		}
		else
		{
			_matrix mScale	= XMMatrixScaling(DataObj.vScale.x, DataObj.vScale.y, DataObj.vScale.z);
			_matrix mAngle = XMMatrixRotationX(XMConvertToRadians(DataObj.vAngle.x)) * XMMatrixRotationY(XMConvertToRadians(DataObj.vAngle.y)) * XMMatrixRotationZ(XMConvertToRadians(DataObj.vAngle.z));
			//_matrix mAngle = XMMatrixRotationX(DataObj.vAngle.x) * XMMatrixRotationY(DataObj.vAngle.y) * XMMatrixRotationZ(DataObj.vAngle.z);
			_matrix mPos	= XMMatrixTranslation(DataObj.vPos.x, DataObj.vPos.y, DataObj.vPos.z);

			_matrix mInstanceLocal = mScale * mAngle * mPos;
			_float4x4 mStoreLocal;
			XMStoreFloat4x4(&mStoreLocal, mInstanceLocal);


			(*iter).second.push_back(mStoreLocal);
		}

	}

	// Instancing
	for (auto& Pair : TempMap)
	{

		CModel_Instance::STATICMODELDESC Desc;
		ZeroMemory(&Desc, sizeof(CStaticModel_Instance::STATICMODELDESC));

		TCHAR szTemp[MAX_PATH];
		string sFileName = Pair.first + "_Instance";
		CToolManager::Get_Instance()->CtoTC(sFileName.data(), szTemp);
		memcpy(Desc.cModelTag, szTemp, sizeof(TCHAR) * MAX_PATH);
		Desc.iNumInstance = Pair.second.size();

		Desc.pLocalInfos = new VTXINSTANCE[Pair.second.size()];

		_uint iIndex = 0;
		for (auto& mLocal : Pair.second)
		{
			memcpy(&Desc.pLocalInfos[iIndex].vRight,	&mLocal.m[0][0], sizeof(_float4));
			memcpy(&Desc.pLocalInfos[iIndex].vUp,		&mLocal.m[1][0], sizeof(_float4));
			memcpy(&Desc.pLocalInfos[iIndex].vLook,		&mLocal.m[2][0], sizeof(_float4));
			memcpy(&Desc.pLocalInfos[iIndex].vPosition, &mLocal.m[3][0], sizeof(_float4));

			++iIndex;
		}




		// 원본 생성
		string sFilePath = "../Bin/Resources/Meshes/NonAnim/" + Pair.first + "/";
		string sFileNameFBX = Pair.first + ".fbx";

		char cTempName[MAX_PATH];
		strcpy(cTempName, Pair.first.data());

		DATA_HEROSCENE* Scene = new DATA_HEROSCENE;

		ZeroMemory(Scene, sizeof(DATA_HEROSCENE));
		ReadSceneData(cTempName, Scene, CDataManager::DATA_NOEANIM);


		TCHAR* pTag = CToolManager::Get_Instance()->Get_ManagedTChar();
		memcpy(pTag, Desc.cModelTag, sizeof(TCHAR) * MAX_PATH);


		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, pTag,
			CModel_Instance::Bin_Create(m_pDevice, m_pContext, Scene, sFilePath.data(), sFileNameFBX.data(), Desc.iNumInstance, Desc.pLocalInfos))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}


		// 바로 생성
		CStaticModel_Instance::STATICMODELDESC ModelInsDesc;
		memcpy(ModelInsDesc.cModelTag, Desc.cModelTag, sizeof(TCHAR) * MAX_PATH);
		if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StaticModel_Instance"), eLEVEL, TEXT("Layer_Model"), &ModelInsDesc)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}




		Safe_Delete_Array(Desc.pLocalInfos);
	}



	RELEASE_INSTANCE(CGameInstance);



	Safe_Delete_Array(pData_Map->pObjDatas);
	Safe_Delete(pData_Map);

	return S_OK;
}


vector<CCell*> CDataManager::Load_Navi(_int iMapID)
{
	vector<CCell*> Cells;

	char cPullName[MAX_PATH];
	char cName[MAX_PATH];
	string ID = to_string(iMapID);

	strcpy_s(cName, "Navi_");
	strcat_s(cName, ID.data());
	strcpy_s(cPullName, "../Bin/ToolData/Map/Navi/");
	strcat_s(cPullName, cName);

	std::ifstream ifs(cPullName, ios::in | ios::binary);


	if (!ifs)
		return Cells;

	_int iID;
	_int iINumCell;
	ifs.read((char*)&iID, sizeof(_int));
	ifs.read((char*)&iINumCell, sizeof(_uint));

	for (_uint i = 0; i < iINumCell; ++i)
	{
		_float3 vPos[3];
		_int iIndex[3];
		ifs.read((char*)&vPos, sizeof(_float3) * 3);
		ifs.read((char*)&iIndex, sizeof(_int) * 3);


		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPos, i, iIndex);
		
		Cells.push_back(pCell);
	}

	return Cells;
}


list<ANIM_LINEAR_DATA> CDataManager::Load_Anim(char * pFileName)
{
	list<ANIM_LINEAR_DATA> Datas;

	char cPullName[MAX_PATH];
	char cName[MAX_PATH];

	strcpy_s(cName, pFileName);
	strcpy_s(cPullName, "../Bin/ToolData/Anim/");
	strcat_s(cPullName, cName);

	std::ifstream ifs(cPullName, ios::in | ios::binary);

	if (!ifs)
		return Datas;

	ifs.read((char*)cName, sizeof(char) * MAX_PATH);
	_int iNum = 0;
	ifs.read((char*)&iNum, sizeof(_int));

	for (_int i = 0; i < iNum; ++i)
	{
		ANIM_LINEAR_DATA TempData;
		ZeroMemory(&TempData, sizeof(ANIM_LINEAR_DATA));
		ifs.read((char*)&TempData, sizeof(ANIM_LINEAR_DATA));
		Datas.push_back(TempData);
	}

	ifs.close();

	return Datas;
}





void CDataManager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
