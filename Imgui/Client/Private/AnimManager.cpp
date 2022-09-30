#include "stdafx.h"
#include "..\Public\AnimManager.h"

#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "MapManager.h"
#include "ToolManager.h"
#include "DataManager.h"

#include "AnimModel.h"
#include "Player.h"

IMPLEMENT_SINGLETON(CAnimManager)

CAnimManager::CAnimManager()
{
}



HRESULT CAnimManager::Init(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	if (nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);

	return S_OK;
}

void CAnimManager::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	

	if (m_bKeyTest)
	{
		if (pGameInstance->Key_Pressing(DIK_A))
		{
			Change_Anim(EDIT_MODEL, m_iA);
		}
		else if (pGameInstance->Key_Pressing(DIK_D))
		{
			Change_Anim(EDIT_MODEL, m_iD);
		}
		else
		{
			Change_Anim(EDIT_MODEL, m_iNone);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}





HRESULT CAnimManager::Create_Try_BinModel(const _tchar * pModelName)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	// 있으면 들어간다
	if (FAILED(pGameInstance->Check_Prototype(LEVEL_ANIMTOOL, pModelName)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}
	RELEASE_INSTANCE(CGameInstance);



	char cTempName[MAX_PATH];
	CToolManager::Get_Instance()->TCtoC(pModelName, cTempName);

	char* tPath = CToolManager::Get_Instance()->Get_ManagedChar();
	strcpy(tPath, "../Bin/Resources/Meshes/Anim/");
	strcat(tPath, cTempName);
	strcat(tPath, "/");

	char* tFileName = CToolManager::Get_Instance()->Get_ManagedChar();
	strcpy(tFileName, cTempName);
	char TempName[MAX_PATH];
	strcpy(TempName, tFileName);
	strcat(tFileName, ".fbx");


	DATA_HEROSCENE* Scene = new DATA_HEROSCENE;
	ZeroMemory(Scene, sizeof(DATA_HEROSCENE));
	_bool bIsBin = true;
	if (FAILED(CDataManager::Get_Instance()->ReadSceneData(TempName, Scene)))
	{
		bIsBin = false;
		Safe_Delete(Scene);
	}



	pGameInstance = GET_INSTANCE(CGameInstance);


	_matrix PivotMatrix;
	PivotMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (bIsBin)
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, pModelName,
			CModel::Bin_Create(m_pDevice, m_pContext, Scene, CModel::TYPE_ANIM, tPath, tFileName, PivotMatrix))))
		{
			// 뭔가 파일 경로가 잘 못 됨.
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, pModelName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, tPath, tFileName, PivotMatrix))))
		{
			// 뭔가 파일 경로가 잘 못 됨.
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


void CAnimManager::Create_Model()
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



	_tchar* tC = CToolManager::Get_Instance()->Get_ManagedTChar();
	CToolManager::Get_Instance()->CtoTC(m_sPickedString.data(), tC);


	if (FAILED(Create_Try_BinModel(tC)))
		return;


	if (m_pAnimModel != nullptr)
	{
		m_pAnimModel->Set_Dead();
		m_pAnimModel = nullptr;
	}

	CAnimModel::ANIMMODELDESC Desc;
	ZeroMemory(&Desc, sizeof(CAnimModel::ANIMMODELDESC));
	memcpy(Desc.cModelTag, tC, sizeof(_tchar)*MAX_PATH);

	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_AnimModel"), LEVEL_ANIMTOOL, TEXT("Layer_AnimModel"), &pObj, &Desc)))
	{
		return;
	}

	m_pAnimModel = (CAnimModel*)pObj;


	RELEASE_INSTANCE(CGameInstance);
}

void CAnimManager::Delete_Model()
{
	if (nullptr == m_pAnimModel)
		return;

	m_pAnimModel->Set_Dead();
	m_pAnimModel = nullptr;
}

void CAnimManager::Create_Player()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	Create_Try_BinModel(TEXT("HatGirl"));

	if (m_pPlayer != nullptr)
	{
		m_pPlayer->Set_Dead();
		m_pPlayer = nullptr;
	}

	CAnimModel::ANIMMODELDESC Desc;
	ZeroMemory(&Desc, sizeof(CAnimModel::ANIMMODELDESC));
	memcpy(Desc.cModelTag, TEXT("HatGirl"), sizeof(_tchar)*MAX_PATH);

	CGameObject* pObj = nullptr;
	pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Player"), LEVEL_ANIMTOOL, TEXT("Layer_Player"), &pObj, &Desc);

	m_pPlayer = (CPlayer*)pObj;


	RELEASE_INSTANCE(CGameInstance);
}

void CAnimManager::Delete_Player()
{
	if (nullptr == m_pPlayer)
		return;

	m_pPlayer->Set_Dead();
	m_pPlayer = nullptr;
}

_float CAnimManager::Get_Player_AnimSpeed(CPlayer::STATE eType)
{
	return m_pPlayer->Get_AnimSpeed(eType);
}

void CAnimManager::Set_Player_AnimSpeed(CPlayer::STATE eType, _float fSpeed)
{
	m_pPlayer->Set_AnimSpeed(eType, fSpeed);
}



_int CAnimManager::Get_AnimCount(EDIT_TYPE eType)
{
	if (EDIT_PLAYER == eType)
		return Get_AnimCount(m_pPlayer);
	else
		return Get_AnimCount(m_pAnimModel);
}

_int CAnimManager::Get_CurAimIndex(EDIT_TYPE eType)
{
	if (EDIT_PLAYER == eType)
		return Get_CurAimIndex(m_pPlayer);
	else
		return Get_CurAimIndex(m_pAnimModel);
}

void CAnimManager::Change_Anim(EDIT_TYPE eType, _int iIndex)
{
	if (EDIT_PLAYER == eType)
		Change_Anim(m_pPlayer, iIndex);
	else
		Change_Anim(m_pAnimModel, iIndex);
}

void CAnimManager::Delete_Anim(EDIT_TYPE eType, _int iIndex)
{
	if (EDIT_PLAYER == eType)
		Delete_Anim(m_pPlayer, iIndex);
	else
		Delete_Anim(m_pAnimModel, iIndex);
}

_float CAnimManager::Get_AnimSpeed(EDIT_TYPE eType)
{
	if (EDIT_PLAYER == eType)
		return Get_AnimSpeed(m_pPlayer);
	else
		return Get_AnimSpeed(m_pAnimModel);
}

void CAnimManager::Set_AnimSpeed(EDIT_TYPE eType, _float fSpeed)
{
	if (EDIT_PLAYER == eType)
		Set_AnimSpeed(m_pPlayer, fSpeed);
	else
		Set_AnimSpeed(m_pAnimModel, fSpeed);
}

_int CAnimManager::Get_AnimCount(CGameObject* pObj)
{
	if (nullptr == pObj)
		return -1;
	return ((CModel*)pObj->Get_ComponentPtr(TEXT("Com_Model")))->Get_AnimIndex();
}

_int CAnimManager::Get_CurAimIndex(CGameObject* pObj)
{
	if (nullptr == pObj)
		return -1;
	return ((CModel*)pObj->Get_ComponentPtr(TEXT("Com_Model")))->Get_CurAnimIndex();
}


void CAnimManager::Change_Anim(CGameObject* pObj, _int iIndex)
{
	if (nullptr == pObj)
		return;

	((CModel*)pObj->Get_ComponentPtr(TEXT("Com_Model")))->Set_AnimIndex(iIndex);
}

void CAnimManager::Delete_Anim(CGameObject* pObj, _int iIndex)
{
	if (nullptr == pObj)
		return;

	((CModel*)pObj->Get_ComponentPtr(TEXT("Com_Model")))->Delete_Anim(iIndex);
}

_float CAnimManager::Get_AnimSpeed(CGameObject * pObj)
{
	return ((CModel*)pObj->Get_ComponentPtr(TEXT("Com_Model")))->Get_CurAin_TickPerSecond();
}

void CAnimManager::Set_AnimSpeed(CGameObject * pObj, _float fSpeed)
{
	((CModel*)pObj->Get_ComponentPtr(TEXT("Com_Model")))->Set_CurAin_TickPerSecond(fSpeed);
}





void CAnimManager::Conv_Bin_Anim()
{
	if (nullptr == m_pAnimModel)
		return;

	char cName[MAX_PATH];
	ZeroMemory(cName, sizeof(char) * MAX_PATH);
	CToolManager::Get_Instance()->TCtoC(m_pAnimModel->Get_ModelName(), cName);

	CDataManager::Get_Instance()->Conv_Bin_Anim((CModel*)m_pAnimModel->Get_ComponentPtr(TEXT("Com_Model")), cName);
}

char * CAnimManager::Get_CurAnimName(EDIT_TYPE eType)
{
	if (EDIT_PLAYER == eType)
		return ((CModel*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Model")))->Get_CurAnim_Name();
	else
		return ((CModel*)m_pAnimModel->Get_ComponentPtr(TEXT("Com_Model")))->Get_CurAnim_Name();
}


void CAnimManager::Set_AnimLinearData()
{
	ANIM_LINEAR_DATA LinearData;
	ZeroMemory(&LinearData, sizeof(ANIM_LINEAR_DATA));
	LinearData.fLimitRatio = m_fLimitRatio;
	LinearData.fTickPerSeconed = m_fTickperSceconed;
	LinearData.iMyIndex = m_iMyIndex;
	LinearData.iTargetIndex = m_iTargetIndex;


	for (auto& Data : m_LinearLists)
	{
		if (m_iMyIndex == Data.iMyIndex && m_iTargetIndex == Data.iTargetIndex)
		{
			Data.fTickPerSeconed = m_fTickperSceconed;
			Data.fLimitRatio = m_fLimitRatio;
			m_pPlayer->Set_AnimLinearData(LinearData);
			return;
		}
	}


	m_pPlayer->Set_AnimLinearData(LinearData);
	m_LinearLists.push_back(LinearData);
}





void CAnimManager::Free()
{

	m_bIsDelete = false;
	m_bKeyTest = false;
	m_sPickedString = "";
	m_bPlayMode = false;
	m_pAnimModel = nullptr;
	m_pPlayer = nullptr;

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
