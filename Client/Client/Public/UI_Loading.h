#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
END

BEGIN(Client)

class CUI_Loading final : public CUI
{
private:
	CUI_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Loading(const CUI_Loading& rhs);
	virtual ~CUI_Loading() = default;


public:
	void CUI_Loading::Set_Start(_bool bStart);


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	HRESULT Ready_Components();


private:
	_bool	m_bStart = false;

private:
	CShader*				m_pShaderCom = nullptr;



public:
	static CUI_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END