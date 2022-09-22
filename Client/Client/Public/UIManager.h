#pragma once

#include "Base.h"

class CUIManager final : public CBase
{
	DECLARE_SINGLETON(CUIManager)

private:
	CUIManager();
	virtual ~CUIManager() = default;

public:



public:
	virtual void Free() override;

};

