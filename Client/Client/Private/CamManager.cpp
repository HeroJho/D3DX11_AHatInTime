#include "stdafx.h"
#include "..\Public\CamManager.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "DataManager.h"
#include "UIManager.h"

#include "Camera_Free.h"

#include "ColorCube.h"
#include "MarkCube.h"
#include "LookCube.h"


IMPLEMENT_SINGLETON(CCamManager)

CCamManager::CCamManager()
{
	m_fMarkMoveSens = 0.1f;
	m_fLookMoveSens = 0.1f;

}





HRESULT CCamManager::Create_Cam()
{
	if (nullptr != m_pCamTool)
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamera::CAMERADESC			CameraDesc;

	CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = ((_float)g_iWinSizeX * 2.f) / (g_iWinSizeY * 2.f);
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.0f;

	CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Camera_Free"), LEVEL_STATIC, TEXT("Layer_Cam"), &pObj, &CameraDesc)))
		return E_FAIL;

	m_pCamTool = (CCamera_Free*)pObj;
	Safe_AddRef(m_pCamTool);

	Safe_Release(pGameInstance);

	return S_OK;
}




















HRESULT CCamManager::Init()
{

	Load_Data();
	return S_OK;
}



void CCamManager::Create_MarkCube(CDataManager::CAMDATA* pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CMarkCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CMarkCube::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(0.f, 1.f, 0.f, 1.f);
	// ColorCubeDesc.vPos = _float3(1.f, 1.f, 1.f);
	ColorCubeDesc.vPos = _float3(0.f, 0.f, 0.f);
	ColorCubeDesc.vScale = _float3(1.f, 1.f, 1.f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MarkCube"), LEVEL_GAMEPLAY, TEXT("Layer_CamTool"), &pTempObj, &ColorCubeDesc)))
		return;

	m_MarkCubes.push_back((CMarkCube*)pTempObj);


	Safe_AddRef(pTempObj);

	if (nullptr != pData)
	{
		((CMarkCube*)pTempObj)->Set_SaveDATA(pData);
	}

	Safe_Release(pGameInstance);

}

void CCamManager::Create_LookCube(CDataManager::CAMDATA* pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CLookCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CLookCube::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(0.f, 1.f, 1.f, 1.f);
	// ColorCubeDesc.vPos = _float3(1.f, 1.f, 1.f);
	ColorCubeDesc.vPos = _float3(0.f, 0.f, 0.f);
	ColorCubeDesc.vScale = _float3(1.f, 1.f, 1.f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_LookCube"), LEVEL_GAMEPLAY, TEXT("Layer_CamTool"), &pTempObj, &ColorCubeDesc)))
		return;

	m_LookCubes.push_back((CLookCube*)pTempObj);

	Safe_AddRef(pTempObj);

	if (nullptr != pData)
	{
		((CLookCube*)pTempObj)->Set_SaveDATA(pData);
	}

	Safe_Release(pGameInstance);

}

void CCamManager::Create_ChaseLookCube()
{
	if (nullptr != m_pChaseLookCube)
	{
		m_pChaseLookCube->Set_Dead(true);
		m_pChaseLookCube = nullptr;
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CColorCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CColorCube::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(0.5f, 0.5f, 0.5f, 1.f);
	ColorCubeDesc.vPos = _float3(0.f, 0.f, 0.f);
	ColorCubeDesc.vScale = _float3(1.f, 1.f, 1.f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_ColorCube"), LEVEL_GAMEPLAY, TEXT("Layer_CamTool"), &pTempObj, &ColorCubeDesc)))
		return;

	m_pChaseLookCube = (CColorCube*)pTempObj;

	Safe_Release(pGameInstance);
}





void CCamManager::Play_CutScene(_uint iIndex, _bool bAutoEnd, CTransform* pTran)
{
	m_bAutoEnd = bAutoEnd;
	m_iCurCutIndex = iIndex;

	CDataManager::DATA_CAMS* Data = m_Datas[iIndex];

	Clear_Data();

	// 절대 좌표
	if (nullptr == pTran)
	{
		for (_uint i = 0; i < Data->iPosNum; ++i)
		{
			Create_MarkCube(&Data->pPosDatas[i]);
		}

		for (_uint i = 0; i < Data->iLookNum; ++i)
		{
			Create_LookCube(&Data->pLookDatas[i]);
		}
	}
	// 상대 좌표
	else
	{
		_matrix mOtherWorld = pTran->Get_WorldMatrix();

		for (_uint i = 0; i < Data->iPosNum; ++i)
		{
			CDataManager::CAMDATA CamData = Data->pPosDatas[i];
			_vector vPos = XMLoadFloat3(&CamData.vPos);
			vPos = XMVector3TransformCoord(vPos, mOtherWorld);
			XMStoreFloat3(&CamData.vPos, vPos);
			Create_MarkCube(&CamData);
		}

		for (_uint i = 0; i < Data->iLookNum; ++i)
		{
			CDataManager::CAMDATA CamData = Data->pLookDatas[i];
			_vector vPos = XMLoadFloat3(&CamData.vPos);
			vPos = XMVector3TransformCoord(vPos, mOtherWorld);
			XMStoreFloat3(&CamData.vPos, vPos);
			Create_LookCube(&CamData);
		}

	}



	Set_Start(true);

}

void CCamManager::End_CutScene()
{
	m_bStart = false;
	m_pCamTool->Set_State(CCamera_Free::CAM_GAME);
}


void CCamManager::Tick(_float fTimeDelta)
{
	_float d = CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_CAM);
	fTimeDelta *= d;

	PlayCutScene(fTimeDelta);
}




void CCamManager::Set_Start(_bool bStart)
{
	m_bStart = bStart;
	m_fMarkSpeed = 0.f;
	m_fMarkTimeAcc = 0.f;
	m_fMarkT = 0.f;
	m_TempMarkCubes.clear();
	m_pTempBasiMarkCubes.clear();

	m_fLookSpeed = 0.f;
	m_fLookTimeAcc = 0.f;
	m_fLookT = 0.f;
	m_TempLookCubes.clear();
	m_pTempBasiLookCubes.clear();

	if (bStart)
	{
		m_TempMarkCubes = m_MarkCubes;
		MakeMarkPos();
		m_vMarkTempPos = CalculMarkBasi();
		m_pCamTool->Set_Pos(m_vMarkTempPos);


		Create_ChaseLookCube();
		m_TempLookCubes = m_LookCubes;
		MakeLookPos();
		m_vLookTempPos = CalculLookBasi();
		m_pChaseLookCube->Set_Pos(m_vLookTempPos);

		m_pCamTool->Set_State(CCamera_Free::CAM_CUTSCENE);
	}

}
void CCamManager::PlayCutScene(_float fTimeDelta)
{
	if (!m_bStart)
		return;

	PlayMark(fTimeDelta);
	PlayLook(fTimeDelta);

}

void CCamManager::PlayMark(_float fTimeDelta)
{

	// 도착 했냐
	if (1.f <= m_fMarkT)
	{
		if (1 > m_TempMarkCubes.size() && m_bAutoEnd)
		{
			m_bStart = false;
			m_pCamTool->Set_State(CCamera_Free::CAM_GAME);


			if (0 == m_iCurCutIndex)
			{
				CUIManager::Get_Instance()->OnOff_HP(true);
				CUIManager::Get_Instance()->OnOff_DiamondScore(true);
				CUIManager::Get_Instance()->OnOff_Inven(true);

				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->PlayBGM(L"NowBGM.mp3", g_fBGMSound);
				RELEASE_INSTANCE(CGameInstance);
			}

			return;
		}
		else if (1 > m_TempMarkCubes.size() && !m_bAutoEnd)
			return;



		// 고른다. 
		m_pTempBasiMarkCubes.clear();
		MakeMarkPos();

		m_fMarkT = 0.f;
		m_fMarkTimeAcc = 0.f;

		return;
	}




	_vector vTempPos = XMLoadFloat3(&m_vMarkTempPos);
	vTempPos = XMVectorSetW(vTempPos, 1.f);

	if (1 == m_pTempBasiMarkCubes.size())
	{
		m_fMarkTimeAcc += fTimeDelta;
		if (m_fMarkSpeed < m_fMarkTimeAcc)
		{
			m_fMarkT = 2.f;
			m_fMarkTimeAcc = 0.f;
		}

	}
	else
	{
		if (m_pCamTool->Move(vTempPos, m_fMarkSpeed, fTimeDelta, 1.f))
		{
			m_fMarkT += m_fMarkMoveSens;
			m_vMarkTempPos = CalculMarkBasi();
		}
	}

	_vector vPos = XMLoadFloat3(&m_pChaseLookCube->Get_Pos());
	vPos = XMVectorSetW(vPos, 1.f);
	m_pCamTool->LookAt(vPos);


}
void CCamManager::PlayLook(_float fTimeDelta)
{
	// 도착 했냐
	if (1.f < m_fLookT)
	{
		if (1 > m_TempLookCubes.size())
		{
			// m_bStart = false;
			return;
		}


		// 고른다. 
		m_pTempBasiLookCubes.clear();
		MakeLookPos();

		m_fLookT = 0.f;
		m_fLookTimeAcc = 0.f;

		m_vLookTempPos = CalculLookBasi();
		m_pChaseLookCube->Set_Pos(m_vLookTempPos);


		return;
	}




	_vector vTempPos = XMLoadFloat3(&m_vLookTempPos);
	vTempPos = XMVectorSetW(vTempPos, 1.f);

	if (1 == m_pTempBasiLookCubes.size())
	{
		m_fLookTimeAcc += fTimeDelta;
		if (m_fLookSpeed < m_fLookTimeAcc)
		{
			m_fLookT = 2.f;
			m_fLookTimeAcc = 0.f;
		}

	}
	else
	{
		if (m_pChaseLookCube->Move(vTempPos, m_fLookSpeed, fTimeDelta, 10.f)) //5.f
		{
			m_fLookT += m_fLookMoveSens;
			m_vLookTempPos = CalculLookBasi();
		}
	}
}


void CCamManager::MakeMarkPos()
{
	if (1 > m_TempMarkCubes.size())
		return;


	list<CMarkCube*>::iterator Cur_iter = m_TempMarkCubes.begin();
	list<CMarkCube*>::iterator Fro_iter = ++(m_TempMarkCubes.begin());

	for (; Cur_iter != m_TempMarkCubes.end(); ++Cur_iter)
	{

		if (1 == m_TempMarkCubes.size())
		{
			m_pTempBasiMarkCubes.push_back(*Cur_iter);
			break;
		}


		_int CurI = (*Cur_iter)->Get_LinkIndex();
		_int FroI = -1;
		if (Fro_iter != m_TempMarkCubes.end())
		{
			FroI = (*Fro_iter)->Get_LinkIndex();
			Fro_iter++;
		}



		if (CurI == FroI)
		{	// 연결 됐다.
			m_pTempBasiMarkCubes.push_back(*Cur_iter);
		}
		else
		{
			m_pTempBasiMarkCubes.push_back(*Cur_iter);
			break;
		}
	}


	for (_int i = 0; i < m_pTempBasiMarkCubes.size(); ++i)
		m_TempMarkCubes.pop_front();

	// 스피드를 설정한다.
	if (0 < m_pTempBasiMarkCubes.size())
	{
		CMarkCube* pTempCube = m_pTempBasiMarkCubes.front();
		m_fMarkSpeed = pTempCube->Get_SpeedTime();
	}
}
void CCamManager::MakeLookPos()
{
	if (1 > m_TempLookCubes.size())
		return;


	list<CLookCube*>::iterator Cur_iter = m_TempLookCubes.begin();
	list<CLookCube*>::iterator Fro_iter = ++(m_TempLookCubes.begin());

	for (; Cur_iter != m_TempLookCubes.end(); ++Cur_iter)
	{

		if (1 == m_TempLookCubes.size())
		{
			m_pTempBasiLookCubes.push_back(*Cur_iter);
			break;
		}


		_int CurI = (*Cur_iter)->Get_LinkIndex();
		_int FroI = -1;
		if (Fro_iter != m_TempLookCubes.end())
		{
			FroI = (*Fro_iter)->Get_LinkIndex();
			Fro_iter++;
		}



		if (CurI == FroI)
		{	// 연결 됐다.
			m_pTempBasiLookCubes.push_back(*Cur_iter);
		}
		else
		{
			m_pTempBasiLookCubes.push_back(*Cur_iter);
			break;
		}
	}


	for (_int i = 0; i < m_pTempBasiLookCubes.size(); ++i)
		m_TempLookCubes.pop_front();

	// 스피드를 설정한다.
	if (0 < m_pTempBasiLookCubes.size())
	{
		CLookCube* pTempCube = m_pTempBasiLookCubes.front();
		m_fLookSpeed = pTempCube->Get_SpeedTime();
	}
}
_float3 CCamManager::CalculMarkBasi()
{
	_int iSize = m_pTempBasiMarkCubes.size();

	_float3 vPos1, vPos2, vPos3, vPos4;

	list<CMarkCube*>::iterator iter = m_pTempBasiMarkCubes.begin();

	switch (iSize)
	{
	case 1:
		vPos1 = (*iter)->Get_Pos();
		return vPos1;
		break;
	case 2:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, m_fMarkT);
	case 3:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		vPos3 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, m_fMarkT);
	case 4:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		vPos3 = (*(++iter))->Get_Pos();
		vPos4 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, vPos4, m_fMarkT);
	}

}
_float3 CCamManager::CalculLookBasi()
{
	_int iSize = m_pTempBasiLookCubes.size();

	_float3 vPos1, vPos2, vPos3, vPos4;

	list<CLookCube*>::iterator iter = m_pTempBasiLookCubes.begin();

	switch (iSize)
	{
	case 1:
		vPos1 = (*iter)->Get_Pos();
		return vPos1;
		break;
	case 2:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, m_fLookT);
	case 3:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		vPos3 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, m_fLookT);
	case 4:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		vPos3 = (*(++iter))->Get_Pos();
		vPos4 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, vPos4, m_fLookT);
	}

}







void CCamManager::Free()
{
	
	Safe_Release(m_pCamTool);

	for (auto& pCube : m_MarkCubes)
		Safe_Release(pCube);
	m_MarkCubes.clear();

	for (auto& pCube : m_LookCubes)
		Safe_Release(pCube);
	m_LookCubes.clear();

	for (auto& pData : m_Datas)
	{
		Safe_Delete_Array(pData->pPosDatas);
		Safe_Delete_Array(pData->pLookDatas);
		Safe_Delete(pData);
	}


	m_Datas.clear();

}





void CCamManager::Load_Data()
{
	CDataManager::DATA_CAMS* Data = nullptr;

	Data = CDataManager::Get_Instance()->Load_Cam(0);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(1);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(2);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(3);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(4);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(5);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(6);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(7);
	m_Datas.push_back(Data);
}





void CCamManager::Clear_PosData()
{
	for (auto& pCube : m_MarkCubes)
		pCube->Set_Dead(true);

	for (auto& pCube : m_MarkCubes)
		Safe_Release(pCube);

	m_MarkCubes.clear();
	m_TempMarkCubes.clear();
	m_pTempBasiMarkCubes.clear();


	m_bStart = false;
	m_fMarkSpeed = 0.f;
	m_fMarkTimeAcc = 0.f;
	m_fMarkT = 0.f;

	m_fLookSpeed = 0.f;
	m_fLookTimeAcc = 0.f;
	m_fLookT = 0.f;

}
void CCamManager::Clear_LookData()
{
	for (auto& pCube : m_LookCubes)
		pCube->Set_Dead(true);

	for (auto& pCube : m_LookCubes)
		Safe_Release(pCube);

	m_LookCubes.clear();
	m_TempLookCubes.clear();
	m_pTempBasiLookCubes.clear();


	m_bStart = false;
	m_fMarkSpeed = 0.f;
	m_fMarkTimeAcc = 0.f;
	m_fMarkT = 0.f;

	m_fLookSpeed = 0.f;
	m_fLookTimeAcc = 0.f;
	m_fLookT = 0.f;
}
void CCamManager::Clear_Data()
{
	Clear_PosData();
	Clear_LookData();
}

