#include "stdafx.h"
#include "..\Public\CamManager.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "ImGui_Manager.h"
#include "DataManager.h"

#include "ColorCube.h"
#include "MarkCube.h"
#include "LookCube.h"
#include "RenderCube.h"
#include "CamSelectingCube.h"
#include "Camera_CamTool.h"



IMPLEMENT_SINGLETON(CCamManager)

CCamManager::CCamManager()
{
	m_fMarkMoveSens = 0.1f;
	m_fLookMoveSens = 0.1f;

}






_float3 CCamManager::Get_SelectingCubePos()
{
	return m_pSelectingCube->Get_Pos();
}

void CCamManager::Set_SelectingCube(_float3 vPos)
{
	_vector vVec = XMLoadFloat3(&vPos);
	vVec = XMVectorSetW(vVec, 1.f);
	m_pSelectingCube->Set_Pos(vVec);
}

_bool CCamManager::Check_Exception()
{
	if (0 == m_MarkCubes.size())
	{
		CImGui_Manager::Get_Instance()->Set_WarningBox("You must have at least one MarkCube!!");
		return true;
	}
	else if (0 == m_LookCubes.size())
	{
		CImGui_Manager::Get_Instance()->Set_WarningBox("You must have at least one LookCube!!");
		return true;
	}

	return false;
}

CGameObject * CCamManager::Get_SelectedCube()
{
	for (auto& pCube : m_MarkCubes)
	{
		if (m_sSelectedMarkCubeTag == pCube->Get_Tag())
		{
			return pCube;
		}

	}

	for (auto& pCube : m_LookCubes)
	{
		if (m_sSelectedMarkCubeTag == pCube->Get_Tag())
		{
			return pCube;
		}
	}

	return nullptr;
}

void CCamManager::Set_SelectingCubePosToSelectedCube()
{
	CGameObject* pObj = Get_SelectedCube();
	if (nullptr == pObj)
		return;

	CTransform* pTran = (CTransform*)pObj->Get_ComponentPtr(TEXT("Com_Transform"));
	
	_vector vVec = pTran->Get_State(CTransform::STATE_POSITION);
	_float3 vPos;
	XMStoreFloat3(&vPos, vVec);
	Set_SelectingCube(vPos);
}

void CCamManager::Set_SelectingCubePosToCamPos()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float3 vPos = *(_float3*)&pGameInstance->Get_CamPosition();
	Set_SelectingCube(vPos);

	RELEASE_INSTANCE(CGameInstance);
}



void CCamManager::Create_CamTool()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamera::CAMERADESC			CameraDesc;

	CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.0f;

	CameraDesc.TransformDesc.fSpeedPerSec = 40.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Camera_CamTool"), LEVEL_CAMTOOL, TEXT("Layer_Cam"), &pTempObj, &CameraDesc)))
		return;

	m_pCamTool = (CCamera_CamTool*)pTempObj;
	Safe_AddRef(m_pCamTool);

	Safe_Release(pGameInstance);
}

void CCamManager::Create_SelectingCube()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamSelectingCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CCamSelectingCube::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(1.f, 0.f, 0.f, 1.f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_CamSelectingCube"), LEVEL_CAMTOOL, TEXT("Layer_CamTool"), &pTempObj, &ColorCubeDesc)))
		return;

	m_pSelectingCube = (CCamSelectingCube*)pTempObj;
	Safe_AddRef(m_pSelectingCube);

	Safe_Release(pGameInstance);
}

void CCamManager::Create_MarkCube(CDataManager::CAMDATA* pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CMarkCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CMarkCube::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(0.f, 1.f, 0.f, 1.f);
	// ColorCubeDesc.vPos = _float3(1.f, 1.f, 1.f);
	ColorCubeDesc.vPos = Get_SelectingCubePos();
	ColorCubeDesc.vScale = _float3(1.f, 1.f, 1.f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MarkCube"), LEVEL_CAMTOOL, TEXT("Layer_CamTool"), &pTempObj, &ColorCubeDesc)))
		return;

	m_MarkCubes.push_back((CMarkCube*)pTempObj);

	string MarkTag = "MarkCube_" + std::to_string(m_iTagIndex);
	pTempObj->Set_Tag(MarkTag);
	++m_iTagIndex;

	Safe_AddRef(pTempObj);

	if (nullptr != pData)
	{
		((CMarkCube*)pTempObj)->Set_SaveDATA(pData);
	}

	Safe_Release(pGameInstance);

	// 포스 계산
	MakeRenderPos();
}

void CCamManager::Create_LookCube(CDataManager::CAMDATA* pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CLookCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CLookCube::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(0.f, 1.f, 1.f, 1.f);
	// ColorCubeDesc.vPos = _float3(1.f, 1.f, 1.f);
	ColorCubeDesc.vPos = Get_SelectingCubePos();
	ColorCubeDesc.vScale = _float3(1.f, 1.f, 1.f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_LookCube"), LEVEL_CAMTOOL, TEXT("Layer_CamTool"), &pTempObj, &ColorCubeDesc)))
		return;

	m_LookCubes.push_back((CLookCube*)pTempObj);

	string LookTag = "LookCube_" + std::to_string(m_iTagIndex);
	pTempObj->Set_Tag(LookTag);
	++m_iTagIndex;

	Safe_AddRef(pTempObj);

	if (nullptr != pData)
	{
		((CLookCube*)pTempObj)->Set_SaveDATA(pData);
	}

	Safe_Release(pGameInstance);

	MakeRenderPos();
}

void CCamManager::Create_RenderPosCube(_float3 vPos, _bool vMark)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CMarkCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CMarkCube::COLORCUBEDESC));

	if (vMark)
		ColorCubeDesc.vColor = _float4(1.f, 1.f, 0.f, 1.f);
	else
		ColorCubeDesc.vColor = _float4(0.f, 1.f, 1.f, 1.f);

	ColorCubeDesc.vPos = vPos;
	ColorCubeDesc.vScale = _float3(.5f, .5f, .5f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_RenderCube"), LEVEL_CAMTOOL, TEXT("Layer_CamTool"), &pTempObj, &ColorCubeDesc)))
		return;

	m_RenderCubes.push_back((CRenderCube*)pTempObj);

	Safe_Release(pGameInstance);
}

void CCamManager::Create_ChaseLookCube()
{
	if (nullptr != m_pChaseLookCube)
	{
		m_pChaseLookCube->Set_Dead();
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

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_ColorCube"), LEVEL_CAMTOOL, TEXT("Layer_CamTool"), &pTempObj, &ColorCubeDesc)))
		return;

	m_pChaseLookCube = (CColorCube*)pTempObj;

	Safe_Release(pGameInstance);
}


void CCamManager::Delete_MarkCube()
{
	for (list<CMarkCube*>::iterator iter = m_MarkCubes.begin(); iter != m_MarkCubes.end();)
	{

		if (m_sSelectedMarkCubeTag == (*iter)->Get_Tag())
		{
			(*iter)->Set_Dead();
			Safe_Release(*iter);
			iter = m_MarkCubes.erase(iter);
		}
		else
			++iter;

	}

	MakeRenderPos();
}

void CCamManager::Delete_LookCube()
{
	for (list<CLookCube*>::iterator iter = m_LookCubes.begin(); iter != m_LookCubes.end();)
	{

		if (m_sSelectedMarkCubeTag == (*iter)->Get_Tag())
		{
			(*iter)->Set_Dead();
			Safe_Release(*iter);
			iter = m_LookCubes.erase(iter);
		}
		else
			++iter;
	}

	MakeRenderPos();
}







void CCamManager::Tick(_float fTimeDelta)
{
	Input(fTimeDelta);

	PlayCutScene(fTimeDelta);
}

void CCamManager::Input(_float fTimeDelta)
{
	if (m_bStart)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);



	if (pGameInstance->Key_Down(DIK_C))
	{
		CCamManager::Get_Instance()->Create_MarkCube();
	}
	else if (pGameInstance->Key_Down(DIK_V))
	{
		CCamManager::Get_Instance()->Create_LookCube();
	}


	Safe_Release(pGameInstance);
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
		if (Check_Exception())
		{
			m_bStart = false;
			return;
		}


		m_TempMarkCubes = m_MarkCubes;
		MakeMarkPos();
		m_vMarkTempPos = CalculMarkBasi();
		m_pCamTool->Set_Pos(m_vMarkTempPos);

		
		Create_ChaseLookCube();
		m_TempLookCubes = m_LookCubes;
		MakeLookPos();
		m_vLookTempPos = CalculLookBasi();
		m_pChaseLookCube->Set_Pos(m_vLookTempPos);
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
		if (1 > m_TempMarkCubes.size())
		{
			m_bStart = false;
			return;
		}


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
		if (m_pChaseLookCube->Move(vTempPos, m_fLookSpeed, fTimeDelta, 5.f))
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







void CCamManager::MakeRenderPos()
{
	m_PlayPosTemp.clear();
	MakeMarkRenderPos();
	MakeLookRenderPos();
}

void CCamManager::MakeMarkRenderPos()
{
	if (1 > m_MarkCubes.size())
		return;

	list<CAMDATA> TempPoss;

	list<CMarkCube*>::iterator Cur_iter = m_MarkCubes.begin();
	list<CMarkCube*>::iterator Fro_iter = ++(m_MarkCubes.begin());

	for (; Cur_iter != m_MarkCubes.end(); ++Cur_iter)
	{
		if (1 == m_MarkCubes.size())
		{
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();
			CamData.bMark = true;

			TempPoss.push_back(CamData);

			// 포스 계산

			CalculRenderBasi(TempPoss);
			break;
		}


		_int CurI = (*Cur_iter)->Get_LinkIndex();
		_int FroI = -1;
		if (Fro_iter != m_MarkCubes.end())
		{
			FroI = (*Fro_iter)->Get_LinkIndex();
			Fro_iter++;
		}



		if (CurI == FroI)
		{	// 연결 됐다.
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();
			CamData.bMark = true;

			TempPoss.push_back(CamData);
		}
		else
		{	
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();
			CamData.bMark = true;

			TempPoss.push_back(CamData);

			// 포스 계산
			CalculRenderBasi(TempPoss);

			TempPoss.clear();
		}
	}
		
	
	RenderPos(1);
}
void CCamManager::MakeLookRenderPos()
{
	if (1 > m_LookCubes.size())
		return;

	list<CAMDATA> TempPoss;

	list<CLookCube*>::iterator Cur_iter = m_LookCubes.begin();
	list<CLookCube*>::iterator Fro_iter = ++(m_LookCubes.begin());

	for (; Cur_iter != m_LookCubes.end(); ++Cur_iter)
	{
		if (1 == m_LookCubes.size())
		{
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();
			CamData.bMark = false;

			TempPoss.push_back(CamData);

			// 포스 계산

			CalculRenderBasi(TempPoss);
			break;
		}


		_int CurI = (*Cur_iter)->Get_LinkIndex();
		_int FroI = -1;
		if (Fro_iter != m_LookCubes.end())
		{
			FroI = (*Fro_iter)->Get_LinkIndex();
			Fro_iter++;
		}



		if (CurI == FroI)
		{	// 연결 됐다.
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();
			CamData.bMark = false;

			TempPoss.push_back(CamData);
		}
		else
		{
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();
			CamData.bMark = false;

			TempPoss.push_back(CamData);

			// 포스 계산
			CalculRenderBasi(TempPoss);

			TempPoss.clear();
		}
	}


	RenderPos(1);
}

void CCamManager::RenderPos(_int iSens)
{
	for (auto& pCube : m_RenderCubes)
		pCube->Set_Dead();
	m_RenderCubes.clear();


	_int iCount = 0;
	_float fCutDis = 0.f;
	_float3 vPrePos;
	ZeroMemory(&vPrePos, sizeof(_float3));
	for (auto& Data : m_PlayPosTemp)
	{
		if (iCount % iSens == 0)
		{
			_vector DisVec = XMLoadFloat3(&vPrePos) - XMLoadFloat3(&Data.vPos);
			fCutDis = XMVectorGetX(XMVector3Length(DisVec));

			if (0.1f < fCutDis)
			{
				Create_RenderPosCube(Data.vPos, Data.bMark);
				vPrePos = Data.vPos;
			}

		}

		++iCount;
	}

}

void CCamManager::CalculRenderBasi(list<CAMDATA> Poss)
{
	_int iSize = Poss.size();

	CAMDATA vPos1, vPos2, vPos3, vPos4;

	switch (iSize)
	{
	case 1:
		vPos1 = Poss.front(); Poss.pop_front();
		GetBesierRenderPos(vPos1);
		break;
	case 2:
		vPos1 = Poss.front(); Poss.pop_front();
		vPos2 = Poss.front(); Poss.pop_front();
		GetBesierRenderPos(vPos1, vPos2);
		break;
	case 3:
		vPos1 = Poss.front(); Poss.pop_front();
		vPos2 = Poss.front(); Poss.pop_front();
		vPos3 = Poss.front(); Poss.pop_front();
		GetBesierRenderPos(vPos1, vPos2, vPos3);
		break;
	case 4:
		vPos1 = Poss.front(); Poss.pop_front();
		vPos2 = Poss.front(); Poss.pop_front();
		vPos3 = Poss.front(); Poss.pop_front();
		vPos4 = Poss.front(); Poss.pop_front();
		GetBesierRenderPos(vPos1, vPos2, vPos3, vPos4);
		break;
	default:
		break;
	}

}
void CCamManager::GetBesierRenderPos(CAMDATA vPos1)
{

}
void CCamManager::GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2)
{
	CAMDATA CamData;
	ZeroMemory(&CamData, sizeof(CAMDATA));

	_float fT = 0.f;
	while (1.f >= fT)
	{
		CamData.vPos = CToolManager::Get_Instance()->GetBesierPos(vPos1.vPos, vPos2.vPos, fT);
		CamData.bMark = vPos1.bMark;

		m_PlayPosTemp.push_back(CamData);
		fT += m_fMarkMoveSens;
	}
}
void CCamManager::GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2, CAMDATA vPos3)
{
	CAMDATA CamData;
	ZeroMemory(&CamData, sizeof(CAMDATA));

	_float fT = 0.f;
	while (1.f >= fT)
	{
		CamData.vPos = CToolManager::Get_Instance()->GetBesierPos(vPos1.vPos, vPos2.vPos, vPos3.vPos, fT);
		CamData.bMark = vPos1.bMark;

		m_PlayPosTemp.push_back(CamData);
		fT += m_fMarkMoveSens;
	}
}
void CCamManager::GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2, CAMDATA vPos3, CAMDATA vPos4)
{
	CAMDATA CamData;
	ZeroMemory(&CamData, sizeof(CAMDATA));

	_float fT = 0.f;
	while (1.f >= fT)
	{
		CamData.vPos = CToolManager::Get_Instance()->GetBesierPos(vPos1.vPos, vPos2.vPos, vPos3.vPos, vPos4.vPos, fT);
		CamData.bMark = vPos1.bMark;

		m_PlayPosTemp.push_back(CamData);
		fT += m_fMarkMoveSens;
	}
}



 



void CCamManager::Free()
{
	Safe_Release(m_pSelectingCube);
	Safe_Release(m_pCamTool);

	for (auto& pCube : m_MarkCubes)
		Safe_Release(pCube);
	m_MarkCubes.clear();

	for (auto& pCube : m_LookCubes)
		Safe_Release(pCube);
	m_LookCubes.clear();

	m_PlayPosTemp.clear();

	m_iTagIndex = 0;
}




void CCamManager::Save_Data()
{
	CDataManager::Get_Instance()->Save_Cam(m_iSaveIndex);
}

void CCamManager::Load_Data()
{

	CDataManager::DATA_CAMS* Data = CDataManager::Get_Instance()->Load_Cam(m_iSaveIndex);

	Clear_Data();

	if (nullptr == Data)
		return;

	for (_uint i = 0; i < Data->iPosNum; ++i)
	{
		Create_MarkCube(&Data->pPosDatas[i]);
	}

	for (_uint i = 0; i < Data->iLookNum; ++i)
	{
		Create_LookCube(&Data->pLookDatas[i]);
	}

}





void CCamManager::Clear_PosData()
{
	for (auto& pCube : m_MarkCubes)
		pCube->Set_Dead();

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
		pCube->Set_Dead();

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


