#include "stdafx.h"
#include "..\Public\CutSceneManager.h"

#include "GameInstance.h"
#include "CamManager.h"
#include "ToolManager.h"


#include "SubconEye.h"
#include "BellMountEye.h"



IMPLEMENT_SINGLETON(CCutSceneManager)

CCutSceneManager::CCutSceneManager()
{
	ZeroMemory(m_pObjs, sizeof(_float) * 100);
}





void CCutSceneManager::Tick(_float fTimeDelta)
{

	switch (m_eState)
	{
	case CUT_CAM3:
		Tick_Cam3(fTimeDelta);
		break;
	case CUT_EYE:
		Tick_Eye(fTimeDelta);
		break;
	case CUT_CAM4:
		Tick_Cam4(fTimeDelta);
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
	case CUT_EYE:
		m_fDisAppearEyeTimeAcc = 0.f;
		m_fEndTimeAcc = 0.f;
		break;
	}

}

void CCutSceneManager::Add_Obj(CGameObject * pObj, _uint iIndex)
{
	m_pObjs[iIndex] = pObj;
}

void CCutSceneManager::Clear_Obj()
{
	ZeroMemory(m_pObjs, sizeof(_float) * 100);
}




void CCutSceneManager::Tick_Cam3(_float fTimeDelta)
{
	m_fCam3_SlowTimeAcc += fTimeDelta;

	if (3.f < m_fCam3_SlowTimeAcc)
	{
		CToolManager::Get_Instance()->Set_All(1.f);
	}

	if (CCamManager::Get_Instance()->Get_IsEnd())
	{
		m_eState = CUT_END;
		Clear_Obj();
	}

}



void CCutSceneManager::Tick_Eye(_float fTimeDelta)
{
	if (nullptr == m_pObjs[0] || nullptr == m_pObjs[1])
		return;

	m_fDisAppearEyeTimeAcc += fTimeDelta;
	if (4.f < m_fDisAppearEyeTimeAcc &&  10.f > m_fDisAppearEyeTimeAcc)
	{
		((CSubconEye*)m_pObjs[0])->StartDisApear();
		((CBellMountEye*)m_pObjs[1])->StartDisApear();

		m_fDisAppearEyeTimeAcc = 11.f;
	}


	m_fEndTimeAcc += fTimeDelta;
	if (10.f < m_fEndTimeAcc)
	{
		m_eState = CUT_END;
		Clear_Obj();
	}


}

void CCutSceneManager::Tick_Cam4(_float fTimeDelta)
{
	if (7 < m_i4Index)
		m_eState = CUT_END;

	if (CCamManager::Get_Instance()->Get_IsEnd())
	{
		CCamManager::Get_Instance()->Play_CutScene(m_i4Index, true);
		++m_i4Index;
	}
}

void CCutSceneManager::Free()
{


}
