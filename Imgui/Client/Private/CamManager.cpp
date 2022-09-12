#include "stdafx.h"
#include "..\Public\CamManager.h"

#include "GameInstance.h"
#include "ToolManager.h"

#include "ColorCube.h"
#include "CamSelectingCube.h"
#include "Camera_CamTool.h"

IMPLEMENT_SINGLETON(CCamManager)

CCamManager::CCamManager()
{
	m_fMoveSens = 0.01f;
}






_float3 CCamManager::Get_SelectingCubePos()
{
	return m_pSelectingCube->Get_Pos();
}

void CCamManager::Set_SelectingCube(_float3 vPos)
{
	m_pSelectingCube->Set_Pos(vPos);
}







void CCamManager::Set_Start(_bool bStart)
{
	m_bStart = bStart; 
	m_fTimeAcc = 0.f; 
	m_fZeroToOneAcc = 0.f;
	m_bDest = true;
	
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
	ColorCubeDesc.iR = 255;
	ColorCubeDesc.iG = 0;
	ColorCubeDesc.iB = 0;

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


	ColorCubeDesc.iR = 0;
	ColorCubeDesc.iG = 0;
	ColorCubeDesc.iB = 255;

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
	MakePos();
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
}








void CCamManager::Tick(_float fTimeDelta)
{
	Input(fTimeDelta);

	PlayCutScene(fTimeDelta);
}

void CCamManager::Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);



	if (pGameInstance->Key_Down(DIK_C))
	{
		CCamManager::Get_Instance()->Create_MarkCube();
	}


	Safe_Release(pGameInstance);
}

void CCamManager::PlayCutScene(_float fTimeDelta)
{
	if (!m_bStart)
		return;

	m_fTimeAcc += fTimeDelta;

}

void CCamManager::MakePos()
{
	m_PlayPosTemp.clear();

	list<_float3> TempPoss;

	list<CColorCube*>::iterator Cur_iter = m_MarkCubes.begin();
	list<CColorCube*>::iterator Fro_iter = ++(m_MarkCubes.begin());

	for (; Cur_iter != m_MarkCubes.end(); ++Cur_iter)
	{
		if (1 == m_MarkCubes.size())
		{
			TempPoss.push_back((*Cur_iter)->Get_Pos());
			CalculBasi(TempPoss);
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
			TempPoss.push_back((*Cur_iter)->Get_Pos());
		}
		else
		{	
			TempPoss.push_back((*Cur_iter)->Get_Pos());

			// 포스 계산
			CalculBasi(TempPoss);

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
	for (auto& vPos : m_PlayPosTemp)
	{ 
		if (iCount % iSens == 0)
		{
			MakeRenderPosCube(vPos);
		}

		++iCount;
	}

}
void CCamManager::MakeRenderPosCube(_float3 vPos)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CColorCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CColorCube::COLORCUBEDESC));

	ColorCubeDesc.iR = 0;
	ColorCubeDesc.iG = 255;
	ColorCubeDesc.iB = 0;

	ColorCubeDesc.vPos = vPos;
	ColorCubeDesc.vScale = _float3(.5f, .5f, .5f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_ColorCube"), LEVEL_CAMTOOL, TEXT("Layer_CamTool"), &pTempObj, &ColorCubeDesc)))
		return;

	m_RenderCubes.push_back((CColorCube*)pTempObj);

	Safe_Release(pGameInstance);
}

void CCamManager::CalculBasi(list<_float3> Poss)
{
	_int iSize = Poss.size();

	_float3 vPos1, vPos2, vPos3, vPos4;

	switch (iSize)
	{
	case 1:
		vPos1 = Poss.front(); Poss.pop_front();
		GetBesierPos(vPos1);
		break;
	case 2:
		vPos1 = Poss.front(); Poss.pop_front();
		vPos2 = Poss.front(); Poss.pop_front();
		GetBesierPos(vPos1, vPos2);
		break;
	case 3:
		vPos1 = Poss.front(); Poss.pop_front();
		vPos2 = Poss.front(); Poss.pop_front();
		vPos3 = Poss.front(); Poss.pop_front();
		GetBesierPos(vPos1, vPos2, vPos3);
		break;
	case 4:
		vPos1 = Poss.front(); Poss.pop_front();
		vPos2 = Poss.front(); Poss.pop_front();
		vPos3 = Poss.front(); Poss.pop_front();
		vPos4 = Poss.front(); Poss.pop_front();
		GetBesierPos(vPos1, vPos2, vPos3, vPos4);
		break;
	default:
		break;
	}

}

void CCamManager::GetBesierPos(_float3 vPos1)
{

}
void CCamManager::GetBesierPos(_float3 vPos1, _float3 vPos2)
{
	_float fT = 0.f;
	while (1.f >= fT)
	{
		m_PlayPosTemp.push_back(CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, fT));
		fT += m_fMoveSens;
	}
}
void CCamManager::GetBesierPos(_float3 vPos1, _float3 vPos2, _float3 vPos3)
{
	_float fT = 0.f;
	while (1.f >= fT)
	{
		m_PlayPosTemp.push_back(CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, fT));
		fT += m_fMoveSens;
	}
}
void CCamManager::GetBesierPos(_float3 vPos1, _float3 vPos2, _float3 vPos3, _float3 vPos4)
{
	_float fT = 0.f;
	while (1.f >= fT)
	{
		m_PlayPosTemp.push_back(CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, vPos4, fT));
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
