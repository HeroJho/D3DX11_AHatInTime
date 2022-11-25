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

class CSplash_wave final : public CGameObject
{
public:
	typedef struct tagSplashWaveDesc
	{
		_float3 vPos;
	}SPLASHWAVEDESC;

private:
	CSplash_wave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSplash_wave(const CSplash_wave& rhs);
	virtual ~CSplash_wave() = default;


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
	CModel*					m_pModelCom = nullptr;


private:
	_float				m_fDeltaTime = 0.f;

	_float				m_fScaleSpeed = 7.f;
	_float				m_fCheckColScale = 2.f;

private:


private:
	HRESULT Ready_Components();


public:
	static CSplash_wave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END