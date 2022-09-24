#include "stdafx.h"
#include "..\Public\DataManager.h"

#include "ToolManager.h"

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

			m_FilePaths.push_back(temp);

		} while (FindNextFile(hFind, &data) != 0);
		
		FindClose(hFind);
	}

	m_FilePaths.pop_front();
	m_FilePaths.pop_front();


	return S_OK;
}

void CDataManager::Free()
{

}
