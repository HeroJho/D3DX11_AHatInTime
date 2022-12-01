#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_DiamondScore final : public CUI
{
protected:
	CUI_DiamondScore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_DiamondScore(const CUI_DiamondScore& rhs);
	virtual ~CUI_DiamondScore() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_Score(_uint iNum);

	void OnOff(_bool bOnOff) { m_bIsOn = bOnOff; m_fTimeAcc = 0.f; }


protected:
	HRESULT Ready_Components();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	_bool			m_bIsOn = false;
	_float2			m_vOnPos;
	_float2			m_vOffPos;

	_float			m_fTimeAcc = 0.f;


public:
	static CUI_DiamondScore* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END