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


public:
	void Dis_MonsterVaultCount();
	_bool Check_MonsterVaultCount();


	// MobCount
private:
	_uint m_iMonsterVaultCount = 1;


public:
	virtual void Free() override;

};

END