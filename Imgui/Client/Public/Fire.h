#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CFire final : public CGameObject
{
private:
	CFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFire(const CFire& rhs);
	virtual ~CFire() = default;


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
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
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


	_float m_fDistortionScale = 0.1f;
	_float m_fDistortionBias = 0.3f;

private:


private:
	HRESULT Ready_Components();


public:
	static CFire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END