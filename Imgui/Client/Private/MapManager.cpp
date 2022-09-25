#include "stdafx.h"
#include "..\Public\MapManager.h"

#include "GameInstance.h"
#include "MapManager.h"
#include "ToolManager.h"

#include "StaticModel.h"



IMPLEMENT_SINGLETON(CMapManager)

CMapManager::CMapManager()
{
}






void CMapManager::Make_Model()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



	CStaticModel::STATICMODELDESC Desc;
	ZeroMemory(&Desc, sizeof(CStaticModel::STATICMODELDESC));

	string sTemp = CMapManager::Get_Instance()->Get_PickedString();
	CToolManager::Get_Instance()->CtoTC(sTemp.data(), Desc.cModelTag);
	
	CGameObject* pObj = nullptr;
	pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StaticModel"), LEVEL_MAPTOOL, TEXT("Layer_Model"), &pObj, &Desc);

	Add_Model((CStaticModel*)pObj);


	RELEASE_INSTANCE(CGameInstance);
}

void CMapManager::Delete_Model()
{
	CStaticModel* pTemp = Find_Model(m_sPickedCreatedString);
	if (nullptr == pTemp)
		return;

	pTemp->Set_Dead();
	m_StaticModels.erase(m_sPickedCreatedString);
}

void CMapManager::Add_Model(CStaticModel * pStaticModel)
{
	string sTag;
	if (GenTag(&sTag))
	{
		// 제대로 Gen이 안됐음 카운트 모자람
	}

	pStaticModel->Set_ModelNum(sTag);
	m_StaticModels.emplace(sTag, pStaticModel);
}

CStaticModel * CMapManager::Get_PickedCreatedModel()
{
	return Find_Model(m_sPickedCreatedString);
}

CStaticModel * CMapManager::Find_Model(string sTag)
{
	_tchar cTempTag[MAX_PATH];
	CToolManager::Get_Instance()->CtoTC(sTag.data(), cTempTag);
	
	map<string, CStaticModel*>::iterator iter = m_StaticModels.find(sTag);

	if (iter == m_StaticModels.end())
		return nullptr;

	return iter->second;
}

bool CMapManager::GenTag(string* pOut)
{
	int iGenNum = 0;
	string sTemp;
	while (iGenNum < 500)
	{
		sTemp = "Model_" + std::to_string(iGenNum);

		if(!Find_Model(sTemp))
		{
			*pOut = sTemp;
			return true;
		}

		++iGenNum;
	}

	return false;
}





void CMapManager::Free()
{

}
