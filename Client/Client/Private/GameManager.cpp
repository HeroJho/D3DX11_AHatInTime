#include "stdafx.h"
#include "..\Public\GameManager.h"
#include "GameInstance.h"



IMPLEMENT_SINGLETON(CGameManager)

CGameManager::CGameManager()
{
}







void CGameManager::Dis_MonsterVaultCount()
{
	--m_iMonsterVaultCount;
	if (0 >= m_iMonsterVaultCount)
		m_iMonsterVaultCount = 0;
}

_bool CGameManager::Check_MonsterVaultCount()
{
	return 0 >= m_iMonsterVaultCount;
}

void CGameManager::Free()
{


}
