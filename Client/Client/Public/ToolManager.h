#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CToolManager final : public CBase
{
	DECLARE_SINGLETON(CToolManager)

public:
	CToolManager();
	virtual ~CToolManager() = default;

public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);


public:
	_bool Get_Debug() { return m_bDebug; }
	void Set_Debug(_bool bDebug) { m_bDebug = bDebug; }

public:
	HRESULT Change_Level(LEVEL eLevel);

public:
	// FOR. Math
	_float3 GetBesierPos(_float3 vPos1, _float3 vPos2, _float fT);
	_float3 GetBesierPos(_float3 vPos1, _float3 vPos2, _float3 vPos3, _float fT);
	_float3 GetBesierPos(_float3 vPos1, _float3 vPos2, _float3 vPos3, _float3 vPos4, _float fT);


public:
	void TCtoC(const TCHAR* pTC, char*pC) { WideCharToMultiByte(CP_ACP, 0, pTC, MAX_PATH, pC, MAX_PATH, NULL, NULL); }
	void CtoTC(const char* pC, TCHAR* pTC) { MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pC, MAX_PATH, pTC, MAX_PATH); }

	_tchar* Get_ManagedTChar();
	char* Get_ManagedChar();

private:
	void ClearManagedChar();

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

private:
	list<_tchar*> m_ManagedTChar;
	list<char*> m_ManagedChar;
	_bool m_bDebug = false;


	// For. TimeManager
public:
	enum TIMETAG { TIME_PLAYER, TIME_MONSTER, TIME_EM, TIME_ALL, TIME_END };

public:
	_float Get_TimeRatio(TIMETAG eTag);
	void Set_TimeRatio(TIMETAG eTag, _float fTimeRatio);

	void Set_WithOutPlayer(_float fTimeRatio);

private:
	_float m_fTimeRatios[TIME_END];



public:
	virtual void Free() override;

};

END

