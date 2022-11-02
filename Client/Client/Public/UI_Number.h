#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_Number final : public CUI
{
protected:
	CUI_Number(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Number(const CUI_Number& rhs);
	virtual ~CUI_Number() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	
public:
	void Set_Number(_uint iNum);

	void Start_Scale();

protected:
	HRESULT Ready_Components();


private:
	_uint m_iNumber = 0;
	string m_sNumber = "";

	_bool	m_bScale = false;
	_float m_fMaxScale = 0.f;
	_float m_fNomalScale = 0.f;
	


private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;



public:
	static CUI_Number* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END