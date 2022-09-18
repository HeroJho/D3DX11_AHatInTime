#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_Panel : public CUI
{
protected:
	CUI_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Panel(const CUI_Panel& rhs);
	virtual ~CUI_Panel() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	HRESULT Ready_Components();

public:
	static CUI_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END