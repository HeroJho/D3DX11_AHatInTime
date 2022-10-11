#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 특정 레벨을 위한 리소스, 객체원형 등을 생성하는 역활을 한다. */
/* 서브스레드릉ㄹ 생성하여 위 작업을 수행하게끔 한다. */

BEGIN(Client)

class CMultiThread final : public CBase
{
public:
	enum LOADINGTYPE { LOADING_READYNEIGHBOR, LOADING_END };

public:
	CMultiThread(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMultiThread() = default;

public:
	_bool Get_Finished() const {
		return m_isFinished;
	}

	CRITICAL_SECTION Get_CS() const {
		return m_CriticalSection;
	}


	_uint Get_iNumReadyNeighbor() { return m_iNumReadyNeighbor; }
	_uint Get_iNumReadyNeighborMax() { return m_iNumReadyNeighborMax; }


public:
	HRESULT Initialize(LOADINGTYPE eLoadingType);
	HRESULT Ready_Neighbor();
	LOADINGTYPE Get_Type() { return m_eLoadingType; };

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

private:
	HANDLE				m_hThread = 0;
	CRITICAL_SECTION	m_CriticalSection;
	LOADINGTYPE			m_eLoadingType = LOADING_END;

private:
	_tchar				m_szLoadingText[MAX_PATH] = TEXT("");
	_bool				m_isFinished = false;

	_uint				m_iNumReadyNeighbor = 0;
	_uint				m_iNumReadyNeighborMax = 0;

public:
	static CMultiThread* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LOADINGTYPE eLoadingType);
	virtual void Free() override;
};

END