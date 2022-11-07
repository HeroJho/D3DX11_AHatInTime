#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_Shop final : public CUI
{
public:
	enum STATE { STATE_OPEN, STATE_IDLE, STATE_BUY, STATE_CLOSE, STATE_CLOSEIDLE, STATE_END };

private:
	CUI_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Shop(const CUI_Shop& rhs);
	virtual ~CUI_Shop() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	void Open();
	void Close();


public:
	void Set_State(STATE eState);

	void Tick_Open(_float fTimeDelta);
	void Tick_Idle(_float fTimeDelta);
	void Tick_Buy(_float fTimeDelta);
	void Tick_Close(_float fTimeDelta);
	void Tick_CloseIdle(_float fTimeDelta);


private:
	HRESULT Ready_Components();

	virtual void Handle_Hover() override;
	virtual void Handle_Down() override;
	virtual void Handle_Press() override;
	virtual void Handle_Click() override;


private:
	_bool m_IsOpen = false;
	STATE m_eState = STATE_END;
	_int m_iSelectedIndex = 0;

	_float2 m_vClosePos;
	_float2 m_vOpenPos;
	_float m_fOnOffSpeed = 0.f;
	
	_float m_fDelayPosY = 0.f;
	_float m_fRatio = 0.f;
	_bool  m_bDelayPosChange = false;
	_bool m_bRatioStart = false;

	_float m_fAlpa = 0.f;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pIdleTextureCom = nullptr;
	CTexture*				m_pBuyTextureCom = nullptr;


public:
	static CUI_Shop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END