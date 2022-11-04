#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CGameManager final : public CBase
{
	DECLARE_SINGLETON(CGameManager)

private:
	CGameManager();
	virtual ~CGameManager() = default;



	// Wisp
public:
	void Set_Wisp(_bool bWispBool, _float fWispRatio, _float3 vWispPos);

	_bool Get_WispBool() { return m_bWispBool; }
	_float Get_WispRatio() { return m_fWispRatio; }
	_float3 Get_WispPos() { return m_vWispPos; }

	_bool Check_IsInWisp(_fvector vPos);
	_bool Check_IsInWispX(_fvector vPos);

private:
	_bool	m_bWispBool = false;
	_float m_fWispRatio = 0.f;
	_float3 m_vWispPos;


	// Diamond
public:
	_uint Get_Diamond() { return m_iDiamond; }

	void Inc_Diamond(_uint iNum);
	void Dec_Diamond(_uint iNum);

private:
	_uint m_iDiamond = 0;


	// Stage_1
public:
	_bool Check_Stage_1();

	void Dis_MonsterVaultCount();
	_bool Check_MonsterVaultCount();


	_bool Get_JumpVault2() { return m_bJumpMapvalut2; }

	void Set_JumpVault() { m_bJumpMapvalut = true; }
	void Set_JumpVault2() { m_bJumpMapvalut2 = true; }

private:
	_uint m_iMonsterVaultCount = 7;
	_bool m_bModVault = false;
	_bool m_bJumpMapvalut = false;
	_bool m_bJumpMapvalut2 = false;



	// For. SavePoint
public:
	void Set_SavePoint(_uint iNaviIndex, _float3 vNaviPos);
	void Get_NaviPoint(_uint* iNaviIndex, _float3* vNaviPos);

private:
	_uint m_iNaviIndex = 0;
	_float3 m_vNaviPos;


public:
	virtual void Free() override;

};

END