#include "stdafx.h"
#include "..\Public\MapManager.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "DataManager.h"
#include "MeshManager.h"

#include "StaticModel.h"
#include "Cell.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CMapManager)

CMapManager::CMapManager()
{
	ZeroMemory(&m_vSubConPos, sizeof(_float3));

	for (_uint i = 0; i < 100; ++i)
		m_CullBoolList[i] = false;

}






HRESULT CMapManager::Init(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

void CMapManager::Tick(_float TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		//if (m_bLightMode)
		//{
		//	if (pGameInstance->Mouse_Pressing(DIMK_WHEEL))
		//	{
		//		Move_ClickedLight();
		//	}
		//}
		//else
		{
			if (pGameInstance->Mouse_Pressing(DIMK_LBUTTON))
				Click_Model();
			else if (pGameInstance->Mouse_Pressing(DIMK_WHEEL))
			{
				Move_ClickedModel();
				// Move_ClickedLight();
			}
			else if (pGameInstance->Mouse_Pressing(DIMK_RBUTTON))
				UnClick_Model();
		}


	}


	RELEASE_INSTANCE(CGameInstance);


	if(m_bColMode)
		Check_ClickedColor();
}

void CMapManager::Make_PickedModel()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



	CStaticModel::STATICMODELDESC Desc;
	ZeroMemory(&Desc, sizeof(CStaticModel::STATICMODELDESC));

	string sTemp = CMapManager::Get_Instance()->Get_PickedString();
	CToolManager::Get_Instance()->CtoTC(sTemp.data(), Desc.cModelTag);
	Desc.vScale = _float3(1.f, 1.f, 1.f);
	Desc.vSize = _float3(1.f, 1.f, 1.f);
	CGameObject* pObj = nullptr;
	pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StaticModel"), LEVEL_MAPTOOL, TEXT("Layer_Model"), &pObj, &Desc);

	Add_Model((CStaticModel*)pObj);


	RELEASE_INSTANCE(CGameInstance);
}

void CMapManager::Make_PickedModel_Pos(_fvector vPos)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



	CStaticModel::STATICMODELDESC Desc;
	ZeroMemory(&Desc, sizeof(CStaticModel::STATICMODELDESC));

	string sTemp = CMapManager::Get_Instance()->Get_PickedString();
	CToolManager::Get_Instance()->CtoTC(sTemp.data(), Desc.cModelTag);
	Desc.vScale = _float3(1.f, 1.f, 1.f);
	XMStoreFloat3(&Desc.vPos, vPos);
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

	if (m_sPickedCreatedString == m_sMinObject)
		m_sMinObject == "";

	pTemp->Set_Dead();
	m_StaticModels.erase(m_sPickedCreatedString);

	m_sPickedCreatedString = "";
}

void CMapManager::Delete_Model_All()
{
	for (auto& Model : m_StaticModels)
		Model.second->Set_Dead();
	m_StaticModels.clear();
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



void CMapManager::Conv_PickedModel_To_Bin()
{

	CStaticModel* pStaticModel = Get_PickedCreatedModel();
	if (nullptr == pStaticModel)
		return;
	CModel* pModel = (CModel*)pStaticModel->Get_ComponentPtr(TEXT("Com_Model"));

	if (pModel->Get_IsBin())
		return;


	char cTemp[MAX_PATH];
	CToolManager::Get_Instance()->TCtoC(pStaticModel->Get_ModelTag(), cTemp);

	
	CDataManager::Get_Instance()->Conv_Bin_Model(pModel, cTemp, CDataManager::DATA_NOEANIM);
}

void CMapManager::Conv_AllModel_To_Bin()
{
	for (auto& pModel : m_StaticModels)
	{
		CStaticModel* pStaticModel = pModel.second;
		if (nullptr == pStaticModel)
			return;
		CModel* pModel = (CModel*)pStaticModel->Get_ComponentPtr(TEXT("Com_Model"));

		if (pModel->Get_IsBin())
			continue;

		char cTemp[MAX_PATH];
		CToolManager::Get_Instance()->TCtoC(pStaticModel->Get_ModelTag(), cTemp);

		CDataManager::Get_Instance()->Conv_Bin_Model(pModel, cTemp, CDataManager::DATA_NOEANIM);
	}


}

void CMapManager::Save_MapData()
{
	CDataManager::Get_Instance()->Save_Map(m_iID);
}

void CMapManager::Load_MapData()
{
	// 기존 Obj를 지운다
	Delete_Model_All();

	CDataManager::DATA_MAP* pMapData = CDataManager::Get_Instance()->Load_Map(m_iID);
	if (nullptr == pMapData)
		return;

	for (_int i = 0; i < pMapData->iNumObj; ++i)
	{
		CDataManager::DATA_MAP_OBJ DataObj = pMapData->pObjDatas[i];

		CStaticModel::STATICMODELDESC Desc;
		ZeroMemory(&Desc, sizeof(CStaticModel::STATICMODELDESC));

		string sTemp = DataObj.cName;
		CToolManager::Get_Instance()->CtoTC(sTemp.data(), Desc.cModelTag);
		Desc.vPos = DataObj.vPos;
		Desc.vAngle = DataObj.vAngle;
		Desc.vScale = DataObj.vScale;

		Desc.vCenter = DataObj.vCenter;
		Desc.vRotation = DataObj.vRotation;
		Desc.vSize = DataObj.vSize;
		Desc.bWall = DataObj.bWall;
		Desc.iTagID = DataObj.iTagID;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pObj = nullptr;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StaticModel"), LEVEL_MAPTOOL, TEXT("Layer_Model"), &pObj, &Desc)))
			return;

		RELEASE_INSTANCE(CGameInstance);

		Add_Model((CStaticModel*)pObj);

	}

	Safe_Delete_Array(pMapData->pObjDatas);
	Safe_Delete(pMapData);

}




void CMapManager::Click_Model()
{
	if ("" != m_sMinObject)
	{
		m_sPickedCreatedString = m_sMinObject;
	}
}

void CMapManager::UnClick_Model()
{
	m_sPickedCreatedString = "";
	m_sMinObject = "";
	ZeroMemory(&m_fMinPos, sizeof(_float3));
}

void CMapManager::Move_ClickedModel()
{
	CStaticModel* pModel = Get_PickedCreatedModel();
	if (nullptr == pModel)
		return;

	CTransform* pTran = (CTransform*)pModel->Get_ComponentPtr(TEXT("Com_Transform"));
	pTran->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_fMinPos), 1.f));
}





void CMapManager::Add_TempClickedModel(_float fMinDis, CGameObject* pObj)
{
	m_TempClickedDis.push_back(fMinDis);
	m_TempClickedObjects.push_back(pObj);
}

void CMapManager::Cul_MinClickedModel()
{

	if (m_TempClickedObjects.empty())
		return;

	_float fMinDis = FLT_MAX_EXP;
	CGameObject* pMinObj = nullptr;

	for (_uint i = 0; i < m_TempClickedDis.size(); ++i)
	{
		if (fMinDis > m_TempClickedDis[i])
		{
			fMinDis = m_TempClickedDis[i];
			pMinObj = m_TempClickedObjects[i];
		}
	}
	m_TempClickedObjects.clear();
	m_TempClickedDis.clear();



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float3 vDir = pGameInstance->Get_MouseDir();
	_float3 vMousePos = pGameInstance->Get_MousePos();

	RELEASE_INSTANCE(CGameInstance);

	_float3 vPos;
	XMStoreFloat3(&vPos, XMLoadFloat3(&vMousePos) + fMinDis * XMVector3Normalize(XMLoadFloat3(&vDir)));

	char cTemp[MAX_PATH];
	
	m_sMinObject = ((CStaticModel*)pMinObj)->Get_ModelNum();
	m_fMinPos = vPos;

}

void CMapManager::AutoGen_ClickedModel()
{
	vector<CCell*>* pCells = CMeshManager::Get_Instance()->Get_Cells();

	_uint iCount = 0;

	for (_uint i = 1000; i < 4000; i+= 4)
	{
		_vector vPos;
		_vector fA = XMLoadFloat3(&(*pCells)[i]->Get_Point(CCell::POINT_A));
		_vector fB = XMLoadFloat3(&(*pCells)[i]->Get_Point(CCell::POINT_B));
		_vector fC = XMLoadFloat3(&(*pCells)[i]->Get_Point(CCell::POINT_C));

		vPos = (fA + fB + fC) / 3.f;

		Make_PickedModel_Pos(vPos);
	}

}

void CMapManager::Add_CulList(string czName)
{
	m_CullList.push_back(czName);
}

_bool CMapManager::Check_CulList(TCHAR* czName)
{
	for (auto& Cull : m_CullList)
	{
		TCHAR szTemp[MAX_PATH];
		char sTemp[MAX_PATH];
		ZeroMemory(szTemp, sizeof(TCHAR) * MAX_PATH);
		ZeroMemory(sTemp, sizeof(char) * MAX_PATH);
		strcpy_s(sTemp, Cull.data());
		CToolManager::Get_Instance()->CtoTC(sTemp, szTemp);

		if (!lstrcmp(czName, szTemp))
			return true;
	}

	return false;
}

void CMapManager::Reset_CulLIst()
{
	m_CullList.clear();
}

_bool* CMapManager::Get_CulBool(_uint iIndex)
{
	return &m_CullBoolList[iIndex];
}

_bool CMapManager::CullingMouse(_fvector vPos)
{
	// 서브콘의 마우스 클릭 위치를 가져온다.
	_vector vSubCon = XMLoadFloat3(&m_vSubConPos);
	_vector vVPos = XMVectorSetY(vPos, XMVectorGetY(vSubCon));
	_float fDis = XMVectorGetX(XMVector3Length(vSubCon - vVPos));

	if (10.f > fDis)
		return true;

	return false;
}




bool CMapManager::GenTag(string* pOut)
{
	int iGenNum = 0;
	string sTemp;
	while (iGenNum < 5000)
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





CCollider::COLLIDERDESC CMapManager::Get_ClikedColDesc()
{
	CStaticModel* pModel = Get_PickedCreatedModel();
	if (nullptr == pModel)
		return CCollider::COLLIDERDESC();

	CCollider::COLLIDERDESC Desc = pModel->Get_ColInfo();

	return Desc;
}

void CMapManager::Set_ClikedColDesc(CCollider::COLLIDERDESC Desc)
{
	CStaticModel* pModel = Get_PickedCreatedModel();
	if (nullptr == pModel)
		return;

	pModel->Set_ColInfo(Desc);
}

void CMapManager::Set_AllSameNameColDesc(CCollider::COLLIDERDESC Desc)
{
	CStaticModel* pModel = Get_PickedCreatedModel();
	if (nullptr == pModel)
		return;

	for (auto& Fair : m_StaticModels)
	{
		if (!lstrcmp(pModel->Get_ModelTag(), Fair.second->Get_ModelTag()))
			Fair.second->Set_ColInfo(Desc);
	}
}

void CMapManager::Check_ClickedColor()
{
	CStaticModel* pModel = Get_PickedCreatedModel();

	if (nullptr == pModel)
		return;

	if (!pModel->Get_Colliders().size())
		return;
	((COBB*)pModel->Get_Colliders().front())->Set_Clicked();
}


_bool CMapManager::Get_ClickedWall()
{
	CCollider::COLLIDERDESC Desc = Get_ClikedColDesc();

	return Desc.bWall;
}

void CMapManager::Set_ClickedWall(_bool bWall)
{
	CCollider::COLLIDERDESC Desc = Get_ClikedColDesc();
	Desc.bWall = bWall;
	Set_ClikedColDesc(Desc);
}

_int CMapManager::Get_ClickedTagID()
{
	CCollider::COLLIDERDESC Desc = Get_ClikedColDesc();

	return Desc.iTagID;
}

void CMapManager::Set_ClickedTagID(_int iTagID)
{
	CCollider::COLLIDERDESC Desc = Get_ClikedColDesc();
	Desc.iTagID = iTagID;
	Set_ClikedColDesc(Desc);
}



void CMapManager::Create_Light()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	ZeroMemory(&m_LightDesc, sizeof(LIGHTDESC));
	m_LightDesc.eType = LIGHTDESC::TYPE_POINT;
	m_LightDesc.vPosition = _float4(-68.97f, 13.04f, 121.20f, 1.f);
	m_LightDesc.fRange = 30.f;
	m_LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	m_LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	m_LightDesc.vSpecular = _float4(0.1f, 0.1f, 0.1f, 1.f);

	CLight* pLight = nullptr;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, m_LightDesc, m_iLgithID, &pLight)))
		return;

	Set_CurLightID(m_iLgithID);

	++m_iLgithID;



	CStaticModel::STATICMODELDESC Desc;
	ZeroMemory(&Desc, sizeof(CStaticModel::STATICMODELDESC));

	lstrcpy(Desc.cModelTag, TEXT("Ori_Hat"));
	Desc.vScale = _float3(1.f, 1.f, 1.f);
	Desc.vSize = _float3(1.f, 1.f, 1.f);
	Desc.pLight = pLight;
	CGameObject* pObj = nullptr;
	pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StaticModel"), LEVEL_MAPTOOL, TEXT("Layer_Model"), &pObj, &Desc);

	CMapManager::Get_Instance()->Add_Model((CStaticModel*)pObj);


	RELEASE_INSTANCE(CGameInstance);
}

void CMapManager::Create_Light(LIGHTDESC eDesc)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, eDesc, m_iLgithID)))
		return;

	Set_CurLightID(m_iLgithID);

	++m_iLgithID;


	RELEASE_INSTANCE(CGameInstance);
}

void CMapManager::Create_Light(LIGHTDESC eDesc, CLight ** pLight)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, eDesc, m_iLgithID, pLight)))
		return;

	Set_CurLightID(m_iLgithID);

	++m_iLgithID;


	RELEASE_INSTANCE(CGameInstance);
}


LIGHTDESC CMapManager::Get_CurIDLight()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CLight* pLight = pGameInstance->Find_Light(m_iCurLightID);

	RELEASE_INSTANCE(CGameInstance);

	if (nullptr == pLight)
		return LIGHTDESC();

	return pLight->Get_LightDesc();
}

void CMapManager::Remove_CurIDLight()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Remove_Light(m_iCurLightID);

	RELEASE_INSTANCE(CGameInstance);
}

void CMapManager::Set_CurIDLight(LIGHTDESC eDesc)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CLight* pLight = pGameInstance->Find_Light(m_iCurLightID);

	RELEASE_INSTANCE(CGameInstance);

	if (nullptr == pLight)
		return;

	pLight->Set_LightDesc(eDesc);
}

list<class CLight*>* CMapManager::Get_Lights()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<class CLight*>* pLights = pGameInstance->Get_Lights();

	RELEASE_INSTANCE(CGameInstance);

	return pLights;
}



void CMapManager::Move_ClickedLight()
{
	LIGHTDESC Desc = Get_CurIDLight();

	Desc.vPosition = _float4(m_fMinPos.x, m_fMinPos.y, m_fMinPos.z, 1.f);

	Set_CurIDLight(Desc);
}

void CMapManager::Save_LightData()
{
	CDataManager::Get_Instance()->Save_Lights(m_iLightSaveIndex);
}

void CMapManager::Load_LightData()
{
	CDataManager::Get_Instance()->Load_Lights(m_iLightSaveIndex);
}





void CMapManager::Free()
{
	m_StaticModels.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
