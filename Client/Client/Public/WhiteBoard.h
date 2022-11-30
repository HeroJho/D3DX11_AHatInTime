#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
END

BEGIN(Client)

class CWhiteBoard final : public CUI
{
private:
	CWhiteBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWhiteBoard(const CWhiteBoard& rhs);
	virtual ~CWhiteBoard() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Set_Start(_bool bStart);

private:
	HRESULT Ready_Components();


private:
	_float m_fTimeAcc = 0.f;
	_float m_fAlpa = 0.f;
	_float m_fAlpaSpeed = 1.f;

	_bool	m_bStart = false;

private:
	CShader*				m_pShaderCom = nullptr;



public:
	static CWhiteBoard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END