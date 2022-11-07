#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_ShopMenu final : public CUI
{
private:
	CUI_ShopMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ShopMenu(const CUI_ShopMenu& rhs);
	virtual ~CUI_ShopMenu() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	void Set_Text(const TCHAR* pText) { lstrcpy(m_pText, pText); }


private:
	HRESULT Ready_Components();

	virtual void Handle_Hover() override;
	virtual void Handle_Down() override;
	virtual void Handle_Press() override;
	virtual void Handle_Click() override;


private:
	TCHAR m_pText[MAX_PATH];


private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;


public:
	static CUI_ShopMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END