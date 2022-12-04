#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CCheckPoint final : public CUI
{
public:
	enum STATE { STATE_ON, STATE_OFF, STATE_END };

private:
	CCheckPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCheckPoint(const CCheckPoint& rhs);
	virtual ~CCheckPoint() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	void Set_OnOff(_bool bOnOff) { m_bOnOff = bOnOff; }


private:
	HRESULT Ready_Components();


private:

	STATE m_eState = STATE_END;
	_float m_fTimeAcc = 0.f;


private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool			m_bOnOff = false;
	_float			m_fOffX = 0.f;
	_float			m_fOnX = 0.f;

public:
	static CCheckPoint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END