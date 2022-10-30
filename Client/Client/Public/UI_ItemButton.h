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
	enum ITEMUISTATE {ITEMUI_START, ITEMUI_IDEL, ITEMUI_EEND, ITEMUI_END};

public:
	typedef struct tagItemButtonDesc
	{
		TCHAR pIconTag[MAX_PATH];
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

public:
	ITEMUISTATE Get_State() { return m_eItemUIState; }
	
	void Start();
	void End();

	void Tick_Start(_float fTimeDelta);
	void Tick_Idel(_float fTimeDelta);
	void Tick_End(_float fTimeDelta);


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
	TCHAR m_pIconTag[MAX_PATH];

	
	ITEMUISTATE				m_eItemUIState = ITEMUI_END;
	_float2 m_vStartPos;
	_float2 m_vEndPos;
	_float  m_fChangeTimeAcc = 0.f;
	_float2 m_vDir;


	_float m_fOriSize = 0.f;
	_float m_fHoverSize = 0.f;

public:
	static CUI_ItemButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END