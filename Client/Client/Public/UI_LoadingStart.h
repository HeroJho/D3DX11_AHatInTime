#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_LoadingStart final : public CUI
{
private:
	CUI_LoadingStart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LoadingStart(const CUI_LoadingStart& rhs);
	virtual ~CUI_LoadingStart() = default;


public:
	void CUI_LoadingStart::Set_Start(_bool bStart);

	_bool Get_RealOff() { return m_bRealOff; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	HRESULT Ready_Components();


private:
	_float m_fTimeAcc = 0.f;
	_float m_fAlpa = 0.f;
	_float m_fAlpaSpeed = 1.f;

	_bool	m_bStart = false;
	_bool	m_bOnOff = false;
	_bool   m_bRealOff = false;

	_float m_fAngle = 0.f;
	_float m_fScale = 0.f;


private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;


public:
	static CUI_LoadingStart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END