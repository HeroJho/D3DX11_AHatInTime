#include "stdafx.h"
#include "..\Public\PartsManager.h"

#include "GameInstance.h"

#include "ToolManager.h"
#include "DataManager.h"

#include "Parts.h"
#include "Player.h"
#include "AnimModel.h"

IMPLEMENT_SINGLETON(CPartsManager)

CPartsManager::CPartsManager()
{
	ZeroMemory(m_BoneName, sizeof(char)*MAX_PATH);
}

HRESULT CPartsManager::Init(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	if (nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);

	return S_OK;
}



void CPartsManager::Create_Parts()
{
	if (nullptr != Find_Parts(m_sPickedString) || "" == m_sPickedString || nullptr == m_pPlayer)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	_tchar* tC = CToolManager::Get_Instance()->Get_ManagedTChar();
	CToolManager::Get_Instance()->CtoTC(m_sPickedString.data(), tC);

	// 원본을 생성한다.
	if (FAILED(CDataManager::Get_Instance()->Create_Try_BinModel(tC, LEVEL_PARTSTOOL, CDataManager::DATA_PARTS)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}



	CParts::PARTSMODELDESC Desc;
	ZeroMemory(&Desc, sizeof(CParts::PARTSMODELDESC));
	memcpy(Desc.cModelTag, tC, sizeof(_tchar)*MAX_PATH);

	CGameObject* pObj = nullptr;


	pObj = m_pPlayer->Add_Sockat(m_BoneName, tC);
	if (nullptr == pObj)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}


	m_Parts.emplace(m_sPickedString, (CParts*)pObj);

	RELEASE_INSTANCE(CGameInstance);
}
void CPartsManager::Delete_Parts()
{
	CParts* pObj = Find_Parts(m_sPickedString);
	if (nullptr ==  pObj|| "" == m_sPickedString)
		return;

	pObj->Set_Dead();
	m_Parts.erase(m_sPickedString);
}


void CPartsManager::Create_Player()
{
	
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("HatGirl"), LEVEL_PARTSTOOL, CDataManager::DATA_ANIM);

	if (m_pPlayer != nullptr)
	{
		m_pPlayer->Set_Dead();
		m_pPlayer = nullptr;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CAnimModel::ANIMMODELDESC Desc;
	ZeroMemory(&Desc, sizeof(CAnimModel::ANIMMODELDESC));
	memcpy(Desc.cModelTag, TEXT("HatGirl"), sizeof(_tchar)*MAX_PATH);

	CGameObject* pObj = nullptr;
	pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Player"), LEVEL_PARTSTOOL, TEXT("Layer_Player"), &pObj, &Desc);

	m_pPlayer = (CPlayer*)pObj;


	RELEASE_INSTANCE(CGameInstance);
}
void CPartsManager::Delete_Player()
{
	if (nullptr == m_pPlayer)
		return;

	m_pPlayer->Set_Dead();
	m_pPlayer = nullptr;
}

void CPartsManager::Conv_AllCratedModel_To_Bin()
{
	for (auto& Part : m_Parts)
	{
		CParts* pPart = Part.second;
		CModel* pModel = (CModel*)pPart->Get_ComponentPtr(TEXT("Com_Model"));
		_tchar* pFileName = pPart->Get_ModelTag();
		char pTempFileName[MAX_PATH];
		CToolManager::Get_Instance()->TCtoC(pFileName, pTempFileName);


		CDataManager::Get_Instance()->Conv_Bin_Model(pModel, pTempFileName, CDataManager::DATA_PARTS);
	}
}

void CPartsManager::Set_StatuMode(_bool bStatu)
{
	m_bStatu = bStatu;
	m_pPlayer->Set_bStatu(bStatu);
}

_uint CPartsManager::Get_AnimIndex()
{
	return m_pPlayer->Get_AnimIndex();
}

void CPartsManager::Set_AnimIndex(_uint iIndex)
{
	m_pPlayer->Set_AnimIndex(iIndex);
}




CParts * CPartsManager::Find_Parts(string sTag)
{
	map<string, CParts*>::iterator iter = m_Parts.find(sTag);

	if (iter == m_Parts.end())
		return nullptr;

	return iter->second;
}




void CPartsManager::Free()
{
	m_Parts.clear();

	// Safe_Release(m_pPlayer);
	m_pPlayer = nullptr;

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
