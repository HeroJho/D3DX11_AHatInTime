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

HRESULT CDataManager::SaveAnimData(CModel * pModel)
{
	DATA_HEROSCENE Scene;
	ZeroMemory(&Scene, sizeof(DATA_HEROSCENE));

	pModel->Get_HierarchyNodeData(&Scene);
	pModel->Get_MaterialData(&Scene);
	pModel->Get_MeshData(&Scene);

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
	
	
	vector<DATA_HEROMETH> temp3;
	for (_int i = 0; i < Scene.iMeshCount; ++i)
	{
		DATA_HEROMETH Hero_Meth;
		ZeroMemory(&Hero_Meth, sizeof(DATA_HEROMETH));

		memcpy(&Hero_Meth, &Scene.pHeroMesh[i], sizeof(DATA_HEROMETH));

		temp3.push_back(Hero_Meth);
	}
	

	Safe_Delete_Array(Scene.pHeroNodes);
	Safe_Delete_Array(Scene.pHeroMaterial);

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
