#include "stdafx.h"
#include "..\Public\CutSceneManager.h"

#include "GameInstance.h"
#include "CamManager.h"
#include "ToolManager.h"



IMPLEMENT_SINGLETON(CCutSceneManager)

CCutSceneManager::CCutSceneManager()
{
}





void CCutSceneManager::Tick(_float fTimeDelta)
{

	switch (m_eState)
	{
	case CUT_CAM3:
		Tick_Cam3(fTimeDelta);
		break;

	default:
		break;
	}

}

void CCutSceneManager::StartCutScene(CUT_STATE eState)
{
	if (m_eState == eState)
		return;

	m_eState = eState;

	switch (m_eState)
	{
	case CUT_CAM3:
		m_fCam3_SlowTimeAcc = 0.f;
		CCamManager::Get_Instance()->Play_CutScene(3, true);
		CToolManager::Get_Instance()->Set_All(0.05f);
		break;
	}

}

void CCutSceneManager::Tick_Cam3(_float fTimeDelta)
{
	m_fCam3_SlowTimeAcc += fTimeDelta;

	if (4.f < m_fCam3_SlowTimeAcc)
	{
		CToolManager::Get_Instance()->Set_All(1.f);
	}

	if (CCamManager::Get_Instance()->Get_IsEnd())
	{
		m_eState = CUT_END;
	}

}



void CCutSceneManager::Free()
{


}
