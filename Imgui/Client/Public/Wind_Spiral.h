#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CTexture;
END

BEGIN(Client)

class CWind_Spiral final : public CGameObject
{
private:
	CWind_Spiral(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWind_Spiral(const CWind_Spiral& rhs);
	virtual ~CWind_Spiral() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Tick_ImGui(_float fTimeDelta) override;


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom0 = nullptr;
	CTexture*				m_pTextureCom1 = nullptr;

private:
	_float				m_fDeltaTime = 0.f;

	_float				m_fScaleSpeed = 1.5f;
	_float				m_fRatio = 0.f;
	_float				m_fMaxScale = 0.f;
	_float				m_fAnlge = 0.f;

	_float3				m_vOriScale;
	_bool m_b = false;

private:


private:
	HRESULT Ready_Components();


public:
	static CWind_Spiral* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END