#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_SelectTool final : public CLevel
{
private:
	CLevel_SelectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_SelectTool() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	static CLevel_SelectTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END