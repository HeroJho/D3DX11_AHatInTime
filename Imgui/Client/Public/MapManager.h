#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CMapManager final : public CBase
{
	DECLARE_SINGLETON(CMapManager)
public:
	CMapManager();
	virtual ~CMapManager() = default;

public:
	_float	Get_Height() { return m_fHeight; }
	void	Set_Height(_float fHeight) { m_fHeight = fHeight; }
	_float	Get_Rad() { return m_fRad; }
	void	Set_Rad(_float fRad) { m_fRad = fRad; }
	_float  Get_Sharp() { return m_fSharp; }
	void	Set_Sharp(_float fSharp) { m_fSharp = fSharp; }

private:
	_float m_fHeight = 0.f;		// 원뿔의 높이
	_float m_fRad = 0.f;		// 원뿔의 반지름
	_float m_fSharp = 0.f;

public:
	virtual void Free() override;

};

END

