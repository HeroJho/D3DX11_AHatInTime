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

HRESULT CDataManager::SaveSceneData(DATA_HEROSCENE * pScene)
{
	std::ofstream ofs("Test.txt", ios::out | ios::binary);

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













#pragma region Test Code

	//vector<DATA_HERONODE> temp;
	//for (_int i = 0; i < ReadScene.iNodeCount; ++i)
	//{
	//	DATA_HERONODE Hero_Node;
	//	ZeroMemory(&Hero_Node, sizeof(DATA_HERONODE));

	//	const char* pMyName = ReadScene.pHeroNodes[i].cName;
	//	const char* pParent = ReadScene.pHeroNodes[i].cParent;
	//	XMFLOAT4X4 mMatrix = ReadScene.pHeroNodes[i].mTransform;
	//	int iDepth = ReadScene.pHeroNodes[i].iDepth;

	//	memcpy(&Hero_Node.cName, pMyName, sizeof(char) * MAX_PATH);
	//	memcpy(&Hero_Node.cParent, pParent, sizeof(char) * MAX_PATH);
	//	Hero_Node.mTransform = mMatrix;
	//	Hero_Node.iDepth = iDepth;

	//	temp.push_back(Hero_Node);
	//}


	//vector<DATA_HEROMATERIAL> temp2;
	//for (_int i = 0; i < ReadScene.iMaterialCount; ++i)
	//{
	//	DATA_HEROMATERIAL Hero_Material;
	//	ZeroMemory(&Hero_Material, sizeof(DATA_HEROMATERIAL));

	//	memcpy(&Hero_Material, &ReadScene.pHeroMaterial[i], sizeof(DATA_HEROMATERIAL));


	//	temp2.push_back(Hero_Material);
	//}

#pragma endregion




	return S_OK;
}

HRESULT CDataManager::ReadSceneData(const char * pFileName, DATA_HEROSCENE* ReadScene)
{

	ZeroMemory(&ReadScene, sizeof(DATA_HEROSCENE));
	std::ifstream ifs("Test.txt", ios::in | ios::binary);

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
			for (int j = 0; j < pHeroMash->NumVertices; ++j)
			{
				VTXANIMMODEL* VtxAniModel = &pHeroMash->pAnimVertices[j];
				ifs.read((char*)VtxAniModel, sizeof(VTXANIMMODEL));
			}
		}
		else
		{
			pHeroMash->pNonAnimVertices = new VTXMODEL[pHeroMash->NumVertices];
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
