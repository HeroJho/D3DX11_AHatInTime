#include "stdafx.h"
#include "..\Public\AnimManager.h"

#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "MapManager.h"
#include "ToolManager.h"
#include "DataManager.h"

#include "AnimModel.h"

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


}





void CAnimManager::Make_Model()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



	// 피킹한 문자열의 모델을 불러온다.
	_tchar* tChar = CToolManager::Get_Instance()->Get_ManagedTChar();
	CToolManager::Get_Instance()->CtoTC(m_sPickedString.data(), tChar);



	// 있는 원본인지 확인한다.
	if (FAILED(pGameInstance->Check_Prototype(LEVEL_ANIMTOOL, tChar)))
	{
		Create_Model();
	}
	else {

		if (!m_bIsLoading)
		{
			m_bIsLoading = true;
			m_cLoadingChar = tChar;
			CImGui_Manager::Get_Instance()->UI_LoadingBox();

			RELEASE_INSTANCE(CGameInstance);
			return;
		}

	}



	RELEASE_INSTANCE(CGameInstance);
}

void CAnimManager::Create_Model()
{
	if (nullptr != m_pAnimModel)
	{
		m_bIsDelete = true;
		// 정말 지울건지 창 띄우기
		return;
	}


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	CAnimModel::ANIMMODELDESC Desc;
	ZeroMemory(&Desc, sizeof(CAnimModel::ANIMMODELDESC));

	string sTemp = m_sPickedString;
	CToolManager::Get_Instance()->CtoTC(sTemp.data(), Desc.cModelTag);

	CGameObject* pObj = nullptr;
	pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_AnimModel"), LEVEL_ANIMTOOL, TEXT("Layer_Model"), &pObj, &Desc);

	m_pAnimModel = (CAnimModel*)pObj;


	RELEASE_INSTANCE(CGameInstance);


	m_bIsDelete = false;
}

void CAnimManager::Load_Model()
{
	if (nullptr == m_cLoadingChar)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	char* tPath = CToolManager::Get_Instance()->Get_ManagedChar();
	strcpy(tPath, "../Bin/Resources/Meshes/Anim/");
	strcat(tPath, m_sPickedString.data());
	strcat(tPath, "/");

	char* tFileName = CToolManager::Get_Instance()->Get_ManagedChar();
	strcpy(tFileName, m_sPickedString.data());
	strcat(tFileName, ".fbx");



	_matrix PivotMatrix;
	PivotMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	// 없다 -> 원본 생성
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, m_cLoadingChar,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, tPath, tFileName, PivotMatrix))))
	{
		// 뭔가 파일 경로가 잘 못 됨.
		RELEASE_INSTANCE(CGameInstance);
		m_bIsLoading = false;
		return;
	}

	m_bIsLoading = false;
	RELEASE_INSTANCE(CGameInstance);
}

void CAnimManager::Delete_Model()
{
	if (nullptr == m_pAnimModel)
		return;

	m_pAnimModel->Set_Dead();
	m_pAnimModel = nullptr;
}



_int CAnimManager::Get_AnimCount()
{
	if (nullptr == m_pAnimModel)
		return -1;

	return m_pAnimModel->Get_AnimCount();
}

_int CAnimManager::Get_CurAimIndex()
{
	if (nullptr == m_pAnimModel)
		return -1;

	return m_pAnimModel->Get_CurAnimIndex();
}

void CAnimManager::Change_Anim(_int iIndex)
{
	if (nullptr == m_pAnimModel)
		return;

	m_pAnimModel->Set_AnimIndex(iIndex);
}

void CAnimManager::Delete_Anim(_int iIndex)
{
	if (nullptr == m_pAnimModel)
		return;

	m_pAnimModel->Delete_Anim(iIndex);
}

void CAnimManager::Save_Anim()
{
	if (nullptr == m_pAnimModel)
		return;

	CDataManager::Get_Instance()->SaveAnimData((CModel*)m_pAnimModel->Get_ComponentPtr(TEXT("Com_Model")));
}





void CAnimManager::Free()
{
	m_pAnimModel = nullptr;

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
