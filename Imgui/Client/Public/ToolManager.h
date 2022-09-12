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

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	
public:
	virtual void Free() override;

};

END

