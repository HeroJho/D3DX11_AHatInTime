#include "stdafx.h"
#include "..\Public\DataManager.h"

#include "ToolManager.h"
#include "Model.h"
#include "HierarchyNode.h"

IMPLEMENT_SINGLETON(CDataManager)

CDataManager::CDataManager()
{
}

HRESULT CDataManager::Init()
{
	if (FAILED(LoadModelPass()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDataManager::SampleSceneData(CModel * pModel)
{
	DATA_HEROSCENE Scene;
	ZeroMemory(&Scene, sizeof(DATA_HEROSCENE));

	pModel->Get_HierarchyNodeData(&Scene);
	pModel->Get_MaterialData(&Scene);
	pModel->Get_MeshData(&Scene);
	pModel->Get_AnimData(&Scene);


#pragma region Test Code

	vector<DATA_HERONODE> temp;
	for (_int i = 0; i < Scene.iNodeCount; ++i)
	{
		DATA_HERONODE Hero_Node;
		ZeroMemory(&Hero_Node, sizeof(DATA_HERONODE));

		const char* pMyName = Scene.pHeroNodes[i].cName;
		const char* pParent = Scene.pHeroNodes[i].cParent;
		XMFLOAT4X4 mMatrix = Scene.pHeroNodes[i].mTransform;

		memcpy(&Hero_Node.cName, pMyName, sizeof(char) * MAX_PATH);
		memcpy(&Hero_Node.cParent, pParent, sizeof(char) * MAX_PATH);
		Hero_Node.mTransform = mMatrix;

		temp.push_back(Hero_Node);
	}


	vector<DATA_HEROMATERIAL> temp2;
	for (_int i = 0; i < Scene.iMaterialCount; ++i)
	{
		DATA_HEROMATERIAL Hero_Material;
		ZeroMemory(&Hero_Material, sizeof(DATA_HEROMATERIAL));
		
		memcpy(&Hero_Material, &Scene.pHeroMaterial[i], sizeof(DATA_HEROMATERIAL));


		temp2.push_back(Hero_Material);
	}


	//vector<DATA_HEROMETH> temp3;
	//for (_int i = 0; i < Scene.iMeshCount; ++i)
	//{
	//	DATA_HEROMETH Hero_Meth;
	//	ZeroMemory(&Hero_Meth, sizeof(DATA_HEROMETH));

	//	memcpy(&Hero_Meth, &Scene.pHeroMesh[i], sizeof(DATA_HEROMETH));

	//	temp3.push_back(Hero_Meth);
	//}
	
#pragma endregion


	SaveSceneData(&Scene);


	Safe_Delete_Array(Scene.pHeroNodes);
	Safe_Delete_Array(Scene.pHeroMaterial);
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

HRESULT CDataManager::SaveSceneData(DATA_HEROSCENE * pScene)
{
	std::fstream ofs("Test.txt", ios::out | ios::binary);

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
		if (nullptr != pMash.pAnimVertices)
		{
			for (int j = 0; j < pMash.NumVertices; ++j)
			{
				VTXANIMMODEL VtxAniModel = pMash.pAnimVertices[j];
				ofs.write((char*)&VtxAniModel, sizeof(VTXANIMMODEL));
			}
		}
		else if(nullptr != pMash.pNonAnimVertices)
		{
			for (int j = 0; j < pMash.NumVertices; ++j)
			{
				VTXMODEL VtxNoneAniModel = pMash.pNonAnimVertices[j];
				ofs.write((char*)&VtxNoneAniModel, sizeof(VTXMODEL));
			}
		}
		

		ofs.write((char*)&pMash.iNumPrimitives, sizeof(int));
		for (int i = 0; i < pMash.iNumPrimitives; ++i)
		{
			FACEINDICES32 Indices32 = pMash.pIndices[i];
			ofs.write((char*)&Indices32, sizeof(FACEINDICES32));
		}
	}



#pragma region Read Code

	DATA_HEROSCENE ReadScene;
	ZeroMemory(&ReadScene, sizeof(DATA_HEROSCENE));
	std::fstream ifs("Test.txt", ios::in | ios::binary);

	if (!ifs)
		return E_FAIL;

	// Node
	int iNodeCount = 0;
	ifs.read((char*)&iNodeCount, sizeof(int));
	ReadScene.iNodeCount = iNodeCount;
	ReadScene.pHeroNodes = new DATA_HERONODE[iNodeCount];
	for (int i = 0; i < iNodeCount; ++i)
	{
		DATA_HERONODE* pHeroNode = &ReadScene.pHeroNodes[i];
		ifs.read((char*)pHeroNode, sizeof(DATA_HERONODE));
	}

	// Material
	int iMaterialCount = 0;
	ifs.read((char*)&iMaterialCount, sizeof(int));
	ReadScene.iMaterialCount = iMaterialCount;
	ReadScene.pHeroMaterial = new DATA_HEROMATERIAL[iMaterialCount];
	for (int i = 0; i < iMaterialCount; ++i)
	{
		DATA_HEROMATERIAL* pHeroMarterial = &ReadScene.pHeroMaterial[i];
		ifs.read((char*)pHeroMarterial, sizeof(DATA_HEROMATERIAL));
	}

	// MashNode
	int iMashCount = 0;
	ifs.read((char*)&iMashCount, sizeof(int));
	ReadScene.iMeshCount = iMashCount;
	ReadScene.pHeroMesh = new DATA_HEROMETH[iMashCount];
	for (int i = 0; i < iMashCount; ++i)
	{
		DATA_HEROMETH* pHeroMash = &ReadScene.pHeroMesh[i];

		ifs.read((char*)&pHeroMash->cName, sizeof(char)*MAX_PATH);
		ifs.read((char*)&pHeroMash->iMaterialIndex, sizeof(int));
		
		ifs.read((char*)&pHeroMash->NumVertices, sizeof(int));

		if()

	}

	
#pragma endregion 






#pragma region Test Code

	vector<DATA_HERONODE> temp;
	for (_int i = 0; i < ReadScene.iNodeCount; ++i)
	{
		DATA_HERONODE Hero_Node;
		ZeroMemory(&Hero_Node, sizeof(DATA_HERONODE));

		const char* pMyName = ReadScene.pHeroNodes[i].cName;
		const char* pParent = ReadScene.pHeroNodes[i].cParent;
		XMFLOAT4X4 mMatrix = ReadScene.pHeroNodes[i].mTransform;

		memcpy(&Hero_Node.cName, pMyName, sizeof(char) * MAX_PATH);
		memcpy(&Hero_Node.cParent, pParent, sizeof(char) * MAX_PATH);
		Hero_Node.mTransform = mMatrix;

		temp.push_back(Hero_Node);
	}


	vector<DATA_HEROMATERIAL> temp2;
	for (_int i = 0; i < ReadScene.iMaterialCount; ++i)
	{
		DATA_HEROMATERIAL Hero_Material;
		ZeroMemory(&Hero_Material, sizeof(DATA_HEROMATERIAL));

		memcpy(&Hero_Material, &ReadScene.pHeroMaterial[i], sizeof(DATA_HEROMATERIAL));


		temp2.push_back(Hero_Material);
	}

#pragma endregion





	int i = 0;

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

void CDataManager::Free()
{

}
