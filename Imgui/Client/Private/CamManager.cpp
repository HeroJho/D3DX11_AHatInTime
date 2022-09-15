#include "stdafx.h"
#include "..\Public\CamManager.h"

#include "GameInstance.h"
#include "ToolManager.h"

#include "ColorCube.h"
#include "RenderCube.h"
#include "CamSelectingCube.h"
#include "Camera_CamTool.h"

IMPLEMENT_SINGLETON(CCamManager)

CCamManager::CCamManager()
{
	m_fMoveSens = 0.03f;
}






_float3 CCamManager::Get_SelectingCubePos()
{
	return m_pSelectingCube->Get_Pos();
}

void CCamManager::Set_SelectingCube(_float3 vPos)
{
	m_pSelectingCube->Set_Pos(vPos);
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

	CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
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

void CCamManager::Create_MarkCube()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CColorCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CColorCube::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(0.f, 1.f, 0.f, 1.f);
	// ColorCubeDesc.vPos = _float3(1.f, 1.f, 1.f);
	ColorCubeDesc.vPos = Get_SelectingCubePos();
	ColorCubeDesc.vScale = _float3(1.f, 1.f, 1.f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_ColorCube"), LEVEL_CAMTOOL, TEXT("Layer_CamTool"), &pTempObj, &ColorCubeDesc)))
		return;

	m_MarkCubes.push_back((CColorCube*)pTempObj);

	string MarkTag = "MarkCube_" + std::to_string(m_iTagIndex);
	pTempObj->Set_Tag(MarkTag);
	++m_iTagIndex;

	Safe_AddRef(pTempObj);

	Safe_Release(pGameInstance);

	// 포스 계산
	MakeRenderPos();
}

void CCamManager::Create_RenderPosCube(_float3 vPos)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CColorCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CColorCube::COLORCUBEDESC));

	ColorCubeDesc.vColor = _float4(1.f, 1.f, 0.f, 1.f);

	ColorCubeDesc.vPos = vPos;
	ColorCubeDesc.vScale = _float3(.5f, .5f, .5f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_RenderCube"), LEVEL_CAMTOOL, TEXT("Layer_CamTool"), &pTempObj, &ColorCubeDesc)))
		return;

	m_RenderCubes.push_back((CRenderCube*)pTempObj);

	Safe_Release(pGameInstance);
}


void CCamManager::Delete_MarkCube()
{
	for (list<CColorCube*>::iterator iter = m_MarkCubes.begin(); iter != m_MarkCubes.end();)
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


	Safe_Release(pGameInstance);
}



void CCamManager::Set_Start(_bool bStart)
{
	m_bStart = bStart;
	m_fSpeed = 0.f;
	m_fTimeAcc = 0.f;
	m_fT = 0.f;

	m_TempMarkCubes.clear();
	m_pTempBasiMarkCubes.clear();

	if (bStart)
	{
		m_TempMarkCubes = m_MarkCubes;
		MakePos();
		m_vTempPos = CalculBasi();
		m_pCamTool->Set_Pos(m_vTempPos);
	}

}
void CCamManager::PlayCutScene(_float fTimeDelta)
{
	if (!m_bStart)
		return;


	// 도착 했냐
	if (1.f < m_fT)
	{
		if (1 > m_TempMarkCubes.size())
		{
			m_bStart = false;
			return;
		}


		// 고른다. 
		m_pTempBasiMarkCubes.clear();
		MakePos();

		m_fT = 0.f;
		m_fTimeAcc = 0.f;

		m_vTempPos = CalculBasi();
		m_pCamTool->Set_Pos(m_vTempPos);
		

		return;
	}
		
	_vector vTempPos = XMLoadFloat3(&m_vTempPos);
	vTempPos = XMVectorSetW(vTempPos, 1.f);
	if (m_pCamTool->Move(vTempPos, m_fSpeed, fTimeDelta, 0.3f))
	{
		m_fT += m_fMoveSens;
		m_vTempPos = CalculBasi();
	}





}



void CCamManager::MakePos()
{
	if (1 > m_TempMarkCubes.size())
		return;


	list<CColorCube*>::iterator Cur_iter = m_TempMarkCubes.begin();
	list<CColorCube*>::iterator Fro_iter = ++(m_TempMarkCubes.begin());

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
		CColorCube* pTempCube = m_pTempBasiMarkCubes.front();
		m_fSpeed = pTempCube->Get_SpeedTime();
	}
}
_float3 CCamManager::CalculBasi()
{
	_int iSize = m_pTempBasiMarkCubes.size();

	_float3 vPos1, vPos2, vPos3, vPos4;

	list<CColorCube*>::iterator iter = m_pTempBasiMarkCubes.begin();

	switch (iSize)
	{
	case 1:
		vPos1 = (*iter)->Get_Pos();
		// CToolManager::Get_Instance()->GetBesierPos(vPos1);
		break;
	case 2:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, m_fT);
	case 3:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		vPos3 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, m_fT);
	case 4:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		vPos3 = (*(++iter))->Get_Pos();
		vPos4 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, vPos4, m_fT);
	}

}




void CCamManager::MakeRenderPos()
{
	if (1 > m_MarkCubes.size())
		return;

	m_PlayPosTemp.clear();

	list<CAMDATA> TempPoss;

	list<CColorCube*>::iterator Cur_iter = m_MarkCubes.begin();
	list<CColorCube*>::iterator Fro_iter = ++(m_MarkCubes.begin());

	for (; Cur_iter != m_MarkCubes.end(); ++Cur_iter)
	{
		if (1 == m_MarkCubes.size())
		{
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();

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

			TempPoss.push_back(CamData);
		}
		else
		{	
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();

			TempPoss.push_back(CamData);

			// 포스 계산
			CalculRenderBasi(TempPoss);

			TempPoss.clear();
		}
	}
		
	
	RenderPos(5);
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
				Create_RenderPosCube(Data.vPos);
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
	CAMDATA CamData;
	ZeroMemory(&CamData, sizeof(CAMDATA));

	_float fT = 0.f;
	while (10.f >= fT)
	{
		CamData.vPos = vPos1.vPos;

		m_PlayPosTemp.push_back(CamData);
		fT += m_fMoveSens;
	}
}
void CCamManager::GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2)
{
	CAMDATA CamData;
	ZeroMemory(&CamData, sizeof(CAMDATA));

	_float fT = 0.f;
	while (1.f >= fT)
	{
		CamData.vPos = CToolManager::Get_Instance()->GetBesierPos(vPos1.vPos, vPos2.vPos, fT);

		m_PlayPosTemp.push_back(CamData);
		fT += m_fMoveSens;
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

		m_PlayPosTemp.push_back(CamData);
		fT += m_fMoveSens;
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

		m_PlayPosTemp.push_back(CamData);
		fT += m_fMoveSens;
	}
}






void CCamManager::Free()
{
	Safe_Release(m_pSelectingCube);
	Safe_Release(m_pCamTool);

	for (auto& pCube : m_MarkCubes)
		Safe_Release(pCube);
	m_MarkCubes.clear();
	m_PlayPosTemp.clear();

	m_iTagIndex = 0;
}

void Create_LookCube()
{
}
