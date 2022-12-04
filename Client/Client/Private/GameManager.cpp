#include "stdafx.h"
#include "..\Public\GameManager.h"
#include "GameInstance.h"

#include "UIManager.h"
#include "ToolManager.h"



IMPLEMENT_SINGLETON(CGameManager)

CGameManager::CGameManager()
{
	ZeroMemory(&m_vNaviPos, sizeof(_float3));
}


void CGameManager::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_uint iNum = Get_WispInfoNum();
	if (!iNum)
	{
		pGameInstance->Set_WipsData(nullptr, nullptr, 0);
		RELEASE_INSTANCE(CGameInstance);
		return;
	}


	_uint iCount = 0;
	for (auto& Wisp : m_WispInfos)
	{
		m_IsDeleteSubCons[iCount] = Wisp.bIsDeleteSubCon;
		m_WispRatios[iCount] = Wisp.fWispRatio;
		m_WispPoss[iCount] = Wisp.vWispPos;

		++iCount;
	}

	// 디퍼드에 정보를 넘겨준다.
	
	pGameInstance->Set_WipsData(m_WispRatios, m_WispPoss, iNum);

	RELEASE_INSTANCE(CGameInstance);

}

void CGameManager::Clear_Data()
{
	m_WispInfos.clear();
	ZeroMemory(m_IsDeleteSubCons, sizeof(_bool) * 256);
	ZeroMemory(m_WispRatios, sizeof(_float) * 256);
	ZeroMemory(m_WispPoss, sizeof(_float3) * 256);
	m_iCurIndex = 0;
}



void CGameManager::Set_Wisp(_bool bIsDeleteSubCon, _float fWispRatio, _float3 vWispPos, _uint iCurIndex)
{
	WISPDESC Desc;
	Desc.bIsDeleteSubCon = bIsDeleteSubCon;
	Desc.fWispRatio = fWispRatio;

	_float4 vPos;
	memcpy(&vPos, &vWispPos, sizeof(_float3));
	vPos.w = 0.f;
	Desc.vWispPos = vPos;

	m_WispInfos.push_back(Desc);
	if(0 != iCurIndex)
		m_iCurIndex = iCurIndex;
}

_bool* CGameManager::Get_DeleteSubCons()
{
	return m_IsDeleteSubCons;
}
_float* CGameManager::Get_WispRatios()
{
	return m_WispRatios;
}
_float4* CGameManager::Get_WispPoss()
{
	return m_WispPoss;
}

_bool CGameManager::Check_IsInWisp(_fvector vPos)
{
	if (!Get_WispInfoNum())
		return false;

	_bool bIsIn = false;
	for(auto& Wisp : m_WispInfos)
	{
		_vector vWispPos = XMLoadFloat4(&Wisp.vWispPos);
		_float fDis = XMVectorGetX(XMVector3Length(vPos - vWispPos));

		if (Wisp.fWispRatio > fDis)
			bIsIn = true;
	}

	return bIsIn;
}

_bool CGameManager::Check_IsInWispX(_fvector vPos)
{
	if (!Get_WispInfoNum())
		return false;

	_bool bIsIn = false;
	for (auto& Wisp : m_WispInfos)
	{
		_vector vWispPos = XMLoadFloat4(&Wisp.vWispPos);
		_vector vVPos = vPos;

		vVPos = XMVectorSetY(vVPos, 0.f);
		vWispPos = XMVectorSetY(vWispPos, 0.f);

		_float fDis = XMVectorGetX(XMVector3Length(vPos - vWispPos));

		if (Wisp.fWispRatio > fDis && Wisp.bIsDeleteSubCon)
			bIsIn = true;
	}

	return bIsIn;
}

_bool CGameManager::Check_IsInWispWithOutSubCon(_fvector vPos)
{
	if (!Get_WispInfoNum())
		return false;

	_bool bIsIn = false;
	for (auto& Wisp : m_WispInfos)
	{
		if (!Wisp.bIsDeleteSubCon)
			continue;
		_vector vWispPos = XMLoadFloat4(&Wisp.vWispPos);
		_float fDis = XMVectorGetX(XMVector3Length(vPos - vWispPos));

		if (Wisp.fWispRatio > fDis)
			bIsIn = true;
	}

	return bIsIn;
}




void CGameManager::Inc_Diamond(_uint iNum)
{
	m_iDiamond += iNum;

	CUIManager::Get_Instance()->OnOff_DiamondScore(true);
	CUIManager::Get_Instance()->Set_Score(m_iDiamond);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->PlaySoundW(L"Pon Pickup.mp3", SOUND_GETITEM, g_fEffectSound);
	RELEASE_INSTANCE(CGameInstance);
}

void CGameManager::Dec_Diamond(_uint iNum)
{
	if (0 < m_iDiamond)
		m_iDiamond -= iNum;
	else
		m_iDiamond = 0;

	CUIManager::Get_Instance()->OnOff_DiamondScore(true);
	CUIManager::Get_Instance()->Set_Score(m_iDiamond);
}




_bool CGameManager::Check_Stage_1()
{
	if(m_bModVault && m_bJumpMapvalut)
		return true;

	return false;
}

void CGameManager::Dis_MonsterVaultCount()
{
	--m_iMonsterVaultCount;
	if (0 >= m_iMonsterVaultCount)
		m_bModVault = true;
}

_bool CGameManager::Check_MonsterVaultCount()
{
	return 0 >= m_iMonsterVaultCount;
}







void CGameManager::Set_SavePoint(_uint iNaviIndex, _float3 vNaviPos)
{
	m_iNaviIndex = iNaviIndex;
	m_vNaviPos = vNaviPos;
	CUIManager::Get_Instance()->Open_CheckPoint();
}

void CGameManager::Get_NaviPoint(_uint* iNaviIndex, _float3* vNaviPos)
{
	*iNaviIndex = m_iNaviIndex;
	*vNaviPos = m_vNaviPos;
}



void CGameManager::Free()
{


}





void CGameManager::Sound_PlayerJump()
{
	if (m_iRandSoundCount < m_iSoundJumpCount)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		_int iInt = CToolManager::Get_Instance()->Get_RendomNum_Int(0, 2);

		switch (iInt)
		{
		case 0:
			pGameInstance->PlaySoundW(L"jump_hut01.ogg", SOUND_PLAYER, g_fTalkSound);
			break;
		case 1:
			pGameInstance->PlaySoundW(L"jump_hut02.ogg", SOUND_PLAYER, g_fTalkSound);
			break;
		case 2:
			pGameInstance->PlaySoundW(L"jump_ya01.ogg", SOUND_PLAYER, g_fTalkSound);
			break;
		default:
			break;
		}


		RELEASE_INSTANCE(CGameInstance);

		m_iSoundJumpCount = 0;

		m_iRandSoundCount = CToolManager::Get_Instance()->Get_RendomNum_Int(1, 3);
	}

	++m_iSoundJumpCount;
}


//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
//pGameInstance->PlaySoundW(L"jump_ya01.ogg", SOUND_PLAYER, g_fTalkSound);
//RELEASE_INSTANCE(CGameInstance);

void CGameManager::Sound_PlayerAttack()
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_int iInt = CToolManager::Get_Instance()->Get_RendomNum_Int(0, 2);

	switch (iInt)
	{
	case 0:
		pGameInstance->PlaySoundW(L"attack_ho01.ogg", SOUND_PLAYER, g_fTalkSound);
		break;
	case 1:
		pGameInstance->PlaySoundW(L"attack_ho02.ogg", SOUND_PLAYER, g_fTalkSound);
		break;
	case 2:
		pGameInstance->PlaySoundW(L"attack_yeah02.ogg", SOUND_PLAYER, g_fTalkSound);
		break;
	default:
		break;
	}


	RELEASE_INSTANCE(CGameInstance);


}

void CGameManager::Sound_PlayerDrow()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_int iInt = CToolManager::Get_Instance()->Get_RendomNum_Int(0, 2);

	switch (iInt)
	{
	case 0:
		pGameInstance->PlaySoundW(L"HatKid_Blammo.ogg", SOUND_PLAYER, g_fTalkSound);
		break;
	case 1:
		pGameInstance->PlaySoundW(L"HatKid_Booyaah.ogg", SOUND_PLAYER, g_fTalkSound);
		break;
	case 2:
		pGameInstance->PlaySoundW(L"HatKid_Bye-bye.ogg", SOUND_PLAYER, g_fTalkSound);
		break;
	default:
		break;
	}


	RELEASE_INSTANCE(CGameInstance);


}
//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
//pGameInstance->PlaySoundW(L"HatKid_Bye-bye.ogg", SOUND_PLAYER, g_fTalkSound);
//RELEASE_INSTANCE(CGameInstance);

void CGameManager::Sound_BGM(_uint iBGM, _bool bForceChange)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if(m_bStop)
		pGameInstance->SetChannelVolume(SOUND_BGM, 0);
	else
		pGameInstance->SetChannelVolume(SOUND_BGM, g_fBGMSound);


	if (m_iCurBGM != iBGM)
	{
		if ((2 == m_iCurBGM || 3 == m_iCurBGM || 4 == m_iCurBGM || 5 == m_iCurBGM) && 0 == iBGM && !bForceChange)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}


		m_iCurBGM = iBGM;
		pGameInstance->StopSound(SOUND_BGM);
		switch (m_iCurBGM)
		{
		case 0:
			pGameInstance->PlayBGM(L"NowBGM.mp3", g_fBGMSound);
			break;
		case 1:
			pGameInstance->PlayBGM(L"OtherBGM.mp3", g_fBGMSound);
			break;
		case 2:
			pGameInstance->PlayBGM(L"BossScreamBGM.mp3", g_fBGMSound);
			break;
		case 3:
			pGameInstance->PlayBGM(L"BossIdleBGM.mp3", g_fBGMSound);
			break;
		case 4:
		{
			_float fD = g_fBGMSound - 0.2f;
			pGameInstance->PlayBGM(L"2StagBGM.mp3", fD);
		}
			break;
		case 5:
			pGameInstance->PlayBGM(L"BossBGM.mp3", g_fBGMSound);
			break;
		default:
			break;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CGameManager::Sound_SnatHurt()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_int iInt = CToolManager::Get_Instance()->Get_RendomNum_Int(0, 2);

	switch (iInt)
	{
	case 0:
		pGameInstance->PlaySoundW(L"Snatcher_IFeelSoWeak.ogg", SOUND_SNAT, g_fSnatSound);
		break;
	case 1:
		pGameInstance->PlaySoundW(L"Snatcher_NOT.ogg", SOUND_SNAT, g_fSnatSound);
		break;
	case 2:
		pGameInstance->PlaySoundW(L"Snatcher_Whoops.ogg", SOUND_SNAT, g_fSnatSound);
		break;
	default:
		break;
	}


	RELEASE_INSTANCE(CGameInstance);

}


void CGameManager::Sound_StopBGM(_bool bBool)
{
	m_bStop = bBool;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_bStop)
		pGameInstance->SetChannelVolume(SOUND_BGM, 0);
	else
		pGameInstance->SetChannelVolume(SOUND_BGM, g_fBGMSound);
	RELEASE_INSTANCE(CGameInstance);

}
