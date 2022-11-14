#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CNavigation;
class CGameObject;

END

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
	HRESULT Change_Level();
	void  Resul_Level(LEVEL eLevel);
	LEVEL Get_CulLevel() { return m_eCulLevel; }

	_bool Get_IsLoading() { return m_bLoading; }
	void Set_IsLoading(_bool bLoading) { m_bLoading = bLoading; }

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
	_bool m_bDebug = false;

	LEVEL m_eLEVEL = LEVEL_END;
	LEVEL m_eCulLevel = LEVEL_GAMEPLAY;
	_bool m_bLoading = false;


	// For. TimeManager
public:
	enum TIMETAG { TIME_PLAYER, TIME_MONSTER, TIME_EM, TIME_CAM, TIME_END };

public:
	_float Get_TimeRatio(TIMETAG eTag);
	void Set_TimeRatio(TIMETAG eTag, _float fTimeRatio);

	void Set_WithOutPlayer(_float fTimeRatio);
	void Set_All(_float fTimeRatio);

private:
	_float m_fTimeRatios[TIME_END];


	// For. Get_Navi
public:
	_uint Find_NaviIndex(_fvector vPos);
	HRESULT Clone_Navi(LEVEL eLevel = LEVEL_GAMEPLAY);
	void Ready_CellCollision(class CGameObject* pObj, _int* iIndexs, _int iSize);

private:
	class CNavigation* m_pNavi = nullptr;


	// For. Font
public:
	void Render_Fonts(const _tchar * pFontTag, const _tchar * pTextm, _float2 vPosition, _fvector vColor, _float fAngle, _float2 vOrigin, _float2 vScale, _bool bSentor = false);
	void Render_FontsY(const _tchar * pFontTag, const _tchar * pTextm, _float2 vPosition, _fvector vColor, _float fAngle, _float2 vOrigin, _float2 vScale);

public:
	virtual void Free() override;

};

END

