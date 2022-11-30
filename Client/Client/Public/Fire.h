#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
class CLight;
END

BEGIN(Client)

class CFire final : public CGameObject
{
public:
	typedef struct tagFireDesc
	{
		_float3 vPos;
		_float3 vScale;

		_float4 vDiffuseColor;
		_float4 vAmColor;

		_float fPower;
		_float fUpSpeed;
		_float fDownSpeed;

		_uint	iIsLight;
	}FIREDESC;

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

	_float m_fDistortionScale = 0.1f;
	_float m_fDistortionBias = 0.3f;


	// ==== Light
	class CLight* m_pLight = nullptr;
	FIREDESC m_Desc;

	_float m_fRatio = 0.f;

	_float m_fTimeAcc = 0.f;
	_float m_fTime = 0.f;

	_bool m_bTurn = false;

private:


private:
	HRESULT Ready_Components();


public:
	static CFire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END