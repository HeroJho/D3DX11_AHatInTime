#pragma once

#include "Client_Defines.h"
#include "Base.h"

class CItemManager final : public CBase
{
	DECLARE_SINGLETON(CItemManager)

private:
	CItemManager();
	virtual ~CItemManager() = default;

public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);


public:
	// � �ƾ����� ��� ���·� ��ġ�Ұų�
	HRESULT Make_Item(const TCHAR* szItemName, LEVEL eLevel, _float3 vPos, _float3 vAngle, _float3 vScale);


private:
	ID3D11Device* m_pDevice			= nullptr;
	ID3D11DeviceContext* m_pContext	= nullptr;


public:
	virtual void Free() override;

};

