#include "stdafx.h"
#include "..\Public\GameManager.h"
#include "GameInstance.h"

#include "UIManager.h"



IMPLEMENT_SINGLETON(CGameManager)

CGameManager::CGameManager()
{
	ZeroMemory(&m_vNaviPos, sizeof(_float3));
}


void CGameManager::Tick(_float fTimeDelta)
{
	_uint iNum = Get_WispInfoNum();
	if (!iNum)
		return;

	_uint iCount = 0;
	for (auto& Wisp : m_WispInfos)
	{
		m_IsDeleteSubCons[iCount] = Wisp.bIsDeleteSubCon;
		m_WispRatios[iCount] = Wisp.fWispRatio;
		m_WispPoss[iCount] = Wisp.vWispPos;
		++iCount;
	}

}

void CGameManager::Clear_Data()
{
	m_WispInfos.clear();
	ZeroMemory(m_IsDeleteSubCons, sizeof(_bool) * 256);
	ZeroMemory(m_WispRatios, sizeof(_float) * 256);
	ZeroMemory(m_WispPoss, sizeof(_float3) * 256);
}



void CGameManager::Set_Wisp(_bool bIsDeleteSubCon, _float fWispRatio, _float3 vWispPos)
{
	WISPDESC Desc;
	Desc.bIsDeleteSubCon = bIsDeleteSubCon;
	Desc.fWispRatio = fWispRatio;

	_float4 vPos;
	memcpy(&vPos, &vWispPos, sizeof(_float3));
	vPos.w = 0.f;
	Desc.vWispPos = vPos;

	m_WispInfos.push_back(Desc);
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
	if(m_bModVault && m_bJumpMapvalut && m_bJumpMapvalut2)
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
}

void CGameManager::Get_NaviPoint(_uint* iNaviIndex, _float3* vNaviPos)
{
	*iNaviIndex = m_iNaviIndex;
	*vNaviPos = m_vNaviPos;
}



void CGameManager::Free()
{


}

