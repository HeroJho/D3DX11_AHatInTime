#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_Health final : public CUI
{
protected:
	CUI_Health(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Health(const CUI_Health& rhs);
	virtual ~CUI_Health() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void OnOff(_bool bOnOff) { m_bIsOn = bOnOff; }

protected:
	HRESULT Ready_Components();

private:
	_bool			m_bIsOn = false;
	_float2			m_vOnPos;
	_float2			m_vOffPos;

	_float			m_fTimeAcc = 0.f;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

public:
	static CUI_Health* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END