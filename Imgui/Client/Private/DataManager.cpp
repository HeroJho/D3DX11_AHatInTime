#include "stdafx.h"
#include "..\Public\DataManager.h"

#include "GameInstance.h"

#include "ToolManager.h"
#include "MapManager.h"

#include "StaticModel.h"


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





HRESULT CDataManager::Conv_Bin_Model(CModel * pModel, char* cModelName, DATA_TYPE eTYPE)
{
	DATA_HEROSCENE Scene;
	ZeroMemory(&Scene, sizeof(DATA_HEROSCENE));

	pModel->Get_HierarchyNodeData(&Scene);
	pModel->Get_MaterialData(&Scene);
	pModel->Get_MeshData(&Scene);
	pModel->Get_AnimData(&Scene);

	SaveSceneData(&Scene, cModelName, eTYPE);

	Safe_Delete_Array(Scene.pHeroNodes);
	Safe_Delete_Array(Scene.pHeroMaterial);

	for (_int i = 0; i < Scene.iMeshCount; ++i)
	{
		Safe_Delete_Array(Scene.pHeroMesh[i].pBones);
	}
	Safe_Delete_Array(Scene.pHeroMesh);


	for (_int i = 0; i < Scene.iNumAnimations; ++i)
	{
		for (_int j = 0; j < Scene.pHeroAnim[i].iNumChannels; ++j) 
		{
			Safe_Delete_Array(Scene.pHeroAnim[i].pHeroChannel[j].pKeyFrames);
		}
		Safe_Delete_Array(Scene.pHeroAnim[i].pHeroChannel);
	}
	Safe_Delete_Array(Scene.pHeroAnim);


	return S_OK;
}
HRESULT CDataManager::SaveSceneData(DATA_HEROSCENE * pScene, char* cModelName, DATA_TYPE eTYPE)
{
	char cPullName[MAX_PATH];
	if(eTYPE == DATA_ANIM)
		strcpy_s(cPullName, "../Bin/ToolData/Bin_Model/Anim/");
	else
		strcpy_s(cPullName, "../Bin/ToolData/Bin_Model/NonAnim/");
	
	strcat_s(cPullName, cModelName);

	std::ofstream ofs(cPullName, ios::out | ios::binary);

	if (!ofs)
		return E_FAIL;
	
	// HierarchyNode
	ofs.write((char*)&pScene->iNodeCount, sizeof(int));
	for (int i = 0; i < pScene->iNodeCount; ++i)
	{
		DATA_HERONODE Node = pScene->pHeroNodes[i];
		ofs.write((char*)&Node, sizeof(DATA_HERONODE));
	}
	// Material
	ofs.write((char*)&pScene->iMaterialCount, sizeof(int));
	for (int i = 0; i < pScene->iMaterialCount; ++i)
	{
		DATA_HEROMATERIAL pMaterial = pScene->pHeroMaterial[i];
		ofs.write((char*)&pMaterial, sizeof(DATA_HEROMATERIAL));
	}

	// MashNode

	ofs.write((char*)&pScene->iMeshCount, sizeof(int));
	for (int i = 0; i < pScene->iMeshCount; ++i)
	{
		DATA_HEROMETH pMash = pScene->pHeroMesh[i];

		ofs.write((char*)&pMash.cName, sizeof(char)*MAX_PATH);
		ofs.write((char*)&pMash.iMaterialIndex, sizeof(int));
		

		ofs.write((char*)&pMash.NumVertices, sizeof(int));
		int iIsAnim = 0;
		if (nullptr != pMash.pAnimVertices)
		{
			iIsAnim = 1;
			ofs.write((char*)&iIsAnim, sizeof(int));

			for (int j = 0; j < pMash.NumVertices; ++j)
			{
				VTXANIMMODEL VtxAniModel = pMash.pAnimVertices[j];
				ofs.write((char*)&VtxAniModel, sizeof(VTXANIMMODEL));
			}
		}
		else if(nullptr != pMash.pNonAnimVertices)
		{
			iIsAnim = 0;
			ofs.write((char*)&iIsAnim, sizeof(int));

			for (int j = 0; j < pMash.NumVertices; ++j)
			{
				VTXMODEL VtxNoneAniModel = pMash.pNonAnimVertices[j];
				ofs.write((char*)&VtxNoneAniModel, sizeof(VTXMODEL));
			}
		}
		

		ofs.write((char*)&pMash.iNumPrimitives, sizeof(int));
		for (int j = 0; j < pMash.iNumPrimitives; ++j)
		{
			FACEINDICES32 Indices32 = pMash.pIndices[j];
			ofs.write((char*)&Indices32, sizeof(FACEINDICES32));
		}

		ofs.write((char*)&pMash.iNumBones, sizeof(int));
		for (int j = 0; j < pMash.iNumBones; ++j)
		{
			DATA_HEROBONE bon = pMash.pBones[j];
			ofs.write((char*)&bon, sizeof(DATA_HEROBONE));
		}
	}
	  

	// Animation
	ofs.write((char*)&pScene->iNumAnimations, sizeof(int));
	for (int i = 0; i < pScene->iNumAnimations; ++i)
	{
		DATA_HEROANIM Anim = pScene->pHeroAnim[i];
		ofs.write((char*)&Anim.iNumChannels, sizeof(int));
		ofs.write((char*)&Anim.fDuration, sizeof(float));
		ofs.write((char*)&Anim.fTickPerSecond, sizeof(float));

		for (int j = 0; j < Anim.iNumChannels; ++j)
		{
			ofs.write((char*)&Anim.pHeroChannel[j].szName, sizeof(char)*MAX_PATH);
			ofs.write((char*)&Anim.pHeroChannel[j].iNumKeyFrames, sizeof(int));

			for (int k = 0; k < Anim.pHeroChannel[j].iNumKeyFrames; ++k)
			{
				ofs.write((char*)&Anim.pHeroChannel[j].pKeyFrames[k], sizeof(KEYFRAME));
			}
		}
	}

	ofs.close();


	return S_OK;
}
HRESULT CDataManager::ReadSceneData(char * pFileName, DATA_HEROSCENE* ReadScene, DATA_TYPE eTYPE)
{
	char cPullName[MAX_PATH];
	if(DATA_ANIM == eTYPE)
		strcpy_s(cPullName, "../Bin/ToolData/Bin_Model/Anim/");
	else
		strcpy_s(cPullName, "../Bin/ToolData/Bin_Model/NonAnim/");

	strcat_s(cPullName, pFileName);


	ZeroMemory(ReadScene, sizeof(DATA_HEROSCENE));
	// strcat(pFileName, ".txt");
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
	if(DATA_ANIM == eTYPE)
		strcpy(tPath, "../Bin/Resources/Meshes/Anim/");
	else
		strcpy(tPath, "../Bin/Resources/Meshes/NonAnim/");

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
	PivotMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	CModel::TYPE etype = CModel::TYPE_END;
	if (DATA_ANIM == eTYPE)
		etype = CModel::TYPE_ANIM;
	else
		etype = CModel::TYPE_NONANIM;
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


	return S_OK;
}






HRESULT CDataManager::Save_Map(_int iMapID)
{
	char cPullName[MAX_PATH];
	char cName[MAX_PATH];
	string ID = to_string(iMapID);
	
	strcpy_s(cName, "MAP_");
	strcat_s(cName, ID.data());
	strcpy_s(cPullName, "../Bin/ToolData/Map/");
	strcat_s(cPullName, cName);


	std::ofstream ofs(cPullName, ios::out | ios::binary);

	if (!ofs)
		return E_FAIL;


	const map<string, CStaticModel*>* pMap = CMapManager::Get_Instance()->Get_StaticModels();
	_int iNumObj = pMap->size();



	ofs.write((char*)&iMapID, sizeof(int));		// ID
	ofs.write((char*)&iNumObj, sizeof(int));	// NumObj

	for (auto& Model : *pMap)
	{
		// Name
		char cTempName[MAX_PATH]; 
		CToolManager::Get_Instance()->TCtoC(Model.second->Get_ModelTag(), cTempName);
		ofs.write((char*)&cTempName, sizeof(char)*MAX_PATH);
		
		// 포 앵 스
		CTransform* pTran = (CTransform*)Model.second->Get_ComponentPtr(TEXT("Com_Transform"));

		_float3 vPos; XMStoreFloat3(&vPos, pTran->Get_State(CTransform::STATE_POSITION));
		_float3 vAngle = *Model.second->Get_Axis();
		_float3 vScale = pTran->Get_Scale();

		ofs.write((char*)&vPos, sizeof(_float3));
		ofs.write((char*)&vAngle, sizeof(_float3));
		ofs.write((char*)&vScale, sizeof(_float3));
	}


	ofs.close();

	return S_OK;
}

CDataManager::DATA_MAP* CDataManager::Load_Map(_int iMapID)
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
		return nullptr;

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
	}

	m_Data_Maps.push_back(pData_Map);
	return pData_Map;
}

HRESULT CDataManager::Save_Anim(char * pFileName, list<ANIM_LINEAR_DATA> Datas)
{
	char cPullName[MAX_PATH];
	char cName[MAX_PATH];

	strcpy_s(cName, pFileName);
	strcpy_s(cPullName, "../Bin/ToolData/Anim/");
	strcat_s(cPullName, cName);


	std::ofstream ofs(cPullName, ios::out | ios::binary);

	if (!ofs)
		return E_FAIL;

	ofs.write((char*)cName, sizeof(char) * MAX_PATH);
	_int iNum = Datas.size();
	ofs.write((char*)&iNum, sizeof(_int));

	for (auto& Data : Datas)
	{
		ofs.write((char*)&Data, sizeof(ANIM_LINEAR_DATA));
	}


	ofs.close();

	return S_OK;
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
	for (auto& MapData : m_Data_Maps)
	{
		Safe_Delete_Array(MapData->pObjDatas);
		Safe_Delete(MapData);
	}



	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
