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

	void Set_JumpVault() { m_bJumpMapvalut = true; }

private:
	_uint m_iMonsterVaultCount = 1;
	_bool m_bModVault = false;
	_bool m_bJumpMapvalut = false;




public:
	virtual void Free() override;

};

END