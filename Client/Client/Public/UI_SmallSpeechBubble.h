#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_SmallSpeechBubble final : public CUI
{
public:
	enum STATE { STATE_ON, STATE_IDLE, STATE_OFF, STATE_REAROFF, STATE_END };

private:
	CUI_SmallSpeechBubble(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SmallSpeechBubble(const CUI_SmallSpeechBubble& rhs);
	virtual ~CUI_SmallSpeechBubble() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	void Tick_On(_float fTimeDelta);
	void Tick_Idle(_float fTimeDelta);
	void Tick_Off(_float fTimeDelta);

public:
	void On_Text(TCHAR* sText, _float fSize, _float fPower, _bool bShake);
	void Set_Text(TCHAR* sText, _float fSize, _float fPower, _bool bShake);
	void Off_Text();

private:
	HRESULT Ready_Components();


private:

	STATE m_eState = STATE_END;
	TCHAR m_sText[MAX_PATH];
	_uint m_iTextCount = 0;
	_float m_fTimeAcc = 0.f;
	_float m_fAlpa = 0.f;

	_bool m_bShake = false;
	_float m_fX = 0;
	_float m_fY = 0;
	_float m_fPower = 0.f;
	_float m_fSize = 1.f;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;



public:
	static CUI_SmallSpeechBubble* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END