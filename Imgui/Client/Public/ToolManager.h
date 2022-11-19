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
	HRESULT Change_Level(LEVEL eLevel);

public:
	// FOR. Math
	_float3 GetBesierPos(_float3 vPos1, _float3 vPos2, _float fT);
	_float3 GetBesierPos(_float3 vPos1, _float3 vPos2, _float3 vPos3, _float fT);
	_float3 GetBesierPos(_float3 vPos1, _float3 vPos2, _float3 vPos3, _float3 vPos4, _float fT);


public:
	void TCtoC(const TCHAR* pTC, char*pC) { WideCharToMultiByte(CP_ACP, 0, pTC, MAX_PATH, pC, MAX_PATH, NULL, NULL);}
	void CtoTC(const char* pC, TCHAR* pTC) { MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pC, MAX_PATH, pTC, MAX_PATH);}

	_tchar* Get_ManagedTChar();
	char* Get_ManagedChar();

	_float Get_RendomNum(_float fMin, _float fMax);
	_int Get_RendomNum_Int(_int iMin, _int iMax);

private:
	void ClearManagedChar();

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

private:
	list<_tchar*> m_ManagedTChar;
	list<char*> m_ManagedChar;

public:
	virtual void Free() override;

};

END

