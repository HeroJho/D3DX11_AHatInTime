#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CItemManager final : public CBase
{
public:
	typedef struct tagHatInfoDesc
	{
		TCHAR szModelName[MAX_PATH];
	}HATINFODESC;

	typedef struct tagItemInfoDesc
	{
		TCHAR szModelName[MAX_PATH];
		_uint iCount;
	}ITEMINFODESC;

	DECLARE_SINGLETON(CItemManager)

private:
	CItemManager();
	virtual ~CItemManager() = default;

public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);


public:
	// 어떤 아아템을 어느 상태로 설치할거냐
	HRESULT Make_Item(const TCHAR* szObjName, const TCHAR* szItemName, LEVEL eLevel, _float3 vPos, _float3 vAngle, _float3 vScale, _uint iCount = 1);



	// For. Inven
public:
	list<HATINFODESC> Get_Hats() { return m_pHats; }
	list<ITEMINFODESC> Get_Items() { return m_pItems; }

public:
	// 이름에 맞는 모자를 추가한다 UI 갱신
	void Add_Hat(TCHAR* szItemName);

	void Add_Item(TCHAR* szItemName, _uint iCount);

private:
	list<HATINFODESC>	m_pHats;
	list<ITEMINFODESC>	m_pItems;



private:
	ID3D11Device* m_pDevice			= nullptr;
	ID3D11DeviceContext* m_pContext	= nullptr;




public:
	virtual void Free() override;

};

END