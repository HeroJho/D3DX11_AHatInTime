#pragma once

#include "Client_Defines.h"
#include "UI_Panel.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_Button final : public CUI_Panel
{
protected:
	CUI_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Button(const CUI_Button& rhs);
	virtual ~CUI_Button() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	HRESULT Ready_Components();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

public:
	static CUI_Button* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END