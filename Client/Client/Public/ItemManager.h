#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CItemManager final : public CBase
{
	DECLARE_SINGLETON(CItemManager)

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


	typedef struct tagSprintDesc
	{
		_float3 vDir;
		_float fDirPow;
		_float fJumpPow;
		_uint iNaviIndex;

		_bool bIsPigic = false;

	}SPRINTDESC;


private:
	CItemManager();
	virtual ~CItemManager() = default;

public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);


	// For. Static
public:
	// � �ƾ����� ��� ���·� ��ġ�Ұų�
	HRESULT Make_Item(const TCHAR* szObjName, const TCHAR* szItemName, LEVEL eLevel, _float3 vPos, _float3 vAngle, _float3 vScale, _uint iCount = 1, void* pArg = nullptr);

	HRESULT Make_DrowItem(const TCHAR* szObjName, const TCHAR* szItemName, LEVEL eLevel, _float3 vPos, _float3 vAngle, _float3 vScale, _uint iItemCount, _float3 vSprDir, _float fSprDirPow, _float fSprJumpPow, _uint iNaviIndex);
	HRESULT Make_PopSprintItem(const TCHAR* szObjName, const TCHAR* szItemName, LEVEL eLevel, _float3 vPos, _float3 vAngle, _float3 vScale, _uint iItemCount, _uint iNaviIndex, _uint iSprCount);

	// For. Player
public:
	// �ع�ġ�� �������� �����.
	HRESULT Make_Hat(TCHAR* pHatModelName, TCHAR* pItemModelName);

	// �ش� �̸��� ���ڷ� ���ڸ� �ٲ۴�.
	void	Change_Hat(TCHAR* pHatModelName);

	// ���� ������ ������.
	HRESULT Make_Flask(_fvector vPos, _fvector vDir, _float fDirPow, _float fJumpPow, _uint iNaviIndex);

	


	// For. Inven
public:
	list<HATINFODESC>* Get_Hats() { return &m_pHats; }
	list<ITEMINFODESC>* Get_Items() { return &m_pItems; }

public:
	// �̸��� �´� ���ڸ� �߰��Ѵ� UI ����
	void Add_Hat(TCHAR* szItemName);
	void Delete_Hat(TCHAR* szItemName);
	_bool Check_Hat(TCHAR* szItemName);

	void Add_Item(TCHAR* szItemName, _uint iCount);

private:
	TCHAR* Match_TextureWithModelName(TCHAR* pTextureName);

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