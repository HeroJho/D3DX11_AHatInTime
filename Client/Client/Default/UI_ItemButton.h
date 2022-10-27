#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_ItemButton final : public CUI
{
public:
	typedef struct tagItemButtonDesc
	{

	}ITEMBUTTONDESC;

private:
	CUI_ItemButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ItemButton(const CUI_ItemButton& rhs);
	virtual ~CUI_ItemButton() = default;

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
	static CUI_ItemButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END