#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_Edit_Button final : public CUI
{
private:
	CUI_Edit_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Edit_Button(const CUI_Edit_Button& rhs);
	virtual ~CUI_Edit_Button() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Components();

	virtual void Handle_Hover() override;
	virtual void Handle_Down() override;
	virtual void Handle_Press() override;
	virtual void Handle_Click() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	UI_STATE				m_eState = UI_END;

public:
	static CUI_Edit_Button* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END