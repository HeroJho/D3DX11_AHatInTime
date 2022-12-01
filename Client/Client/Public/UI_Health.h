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
	_uint Get_Hp() { return m_iHp; }
	void Set_Hp(_uint iHp) { m_iHp = iHp; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void OnOff(_bool bOnOff) { m_bIsOn = bOnOff; m_fTimeAcc = 0.f; }


	void DangerTick(_float fTimeDelta);

protected:
	HRESULT Ready_Components();

private:
	_bool			m_bIsOn = false;
	_float2			m_vOnPos;
	_float2			m_vOffPos;

	_float			m_fTimeAcc = 0.f;
	_uint			m_iHp = 0;


	_float m_fRaito = 0.f;
	_float m_fOriSizeX = 0.f;
	_float m_fOriSizeY = 0.f;
	_float	m_fIdleMaxSizeX = 0.f;
	_float	m_fIdleMaxSizeY = 0.f;
	_float	m_fIdleSpeed = 0.f;
	_bool	m_bIdleChange = false;


private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

public:
	static CUI_Health* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END