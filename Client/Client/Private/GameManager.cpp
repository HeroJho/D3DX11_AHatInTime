#include "stdafx.h"
#include "..\Public\GameManager.h"
#include "GameInstance.h"

#include "UIManager.h"



IMPLEMENT_SINGLETON(CGameManager)

CGameManager::CGameManager()
{
}







void CGameManager::Set_Wisp(_bool bWispBool, _float fWispRatio, _float3 vWispPos)
{
	m_bWispBool = bWispBool;

	m_fWispRatio = fWispRatio;
	m_vWispPos = vWispPos;
}

_bool CGameManager::Check_IsInWisp(_fvector vPos)
{
	if (!m_bWispBool)
		return false;

	_vector vWispPos = XMLoadFloat3(&m_vWispPos);
	_float fDis = XMVectorGetX(XMVector3Length(vPos - vWispPos));

	if (m_fWispRatio > fDis)
		return true;

	return false;
}

void CGameManager::Inc_Diamond(_uint iNum)
{
	m_iDiamond += iNum;

	CUIManager::Get_Instance()->Set_Score(m_iDiamond);
}

void CGameManager::Dec_Diamond(_uint iNum)
{
	if (0 < m_iDiamond)
		m_iDiamond -= iNum;
	else
		m_iDiamond = 0;

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

void CGameManager::Free()
{


}
