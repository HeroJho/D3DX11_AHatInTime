#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CTestManager final : public CBase
{
	DECLARE_SINGLETON(CTestManager)
public:
	CTestManager();
	virtual ~CTestManager() = default;

public:
	int Get_iTest() { return m_iTest; }
	void Set_iTest(int i) { m_iTest += i; }

private:
	int m_iTest = 0;

public:
	virtual void Free() override;

};

END

