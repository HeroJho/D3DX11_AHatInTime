#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CParticle final : public CGameObject
{
public:
	enum PARTICLE_TYPE { TYPE_MODLE, TYPE_TEXTURE, TYPE_END };

public:
	typedef struct tagPartileDesc
	{
		PARTICLE_TYPE			eType;
		TCHAR					cModelTag[MAX_PATH];

		_float3 vParentPos;

		_float3 vPos;
		_float3 vLook;
		_float3 vScale;
		_float3 vRoation;

		_float3 vDir;
		_float fStopValue = 0.f;
		_float fSpeed = 0.f;

		_float3 vScaleSpeed;

		_bool	bGravity = false;
		_float fJumpPower = 0.f;
		_float fGravityValue = 0.f;



		_float fLifeTime = 0.f;

	}PARTICLEDESC;

private:
	CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle(const CParticle& rhs);
	virtual ~CParticle() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	void Tick_Model(_float fTimeDelta);
	void Tick_Texture(_float fTimeDelta);

	void LateTick_Model(_float fTimeDelta);
	void LateTick_Texture(_float fTimeDelta);

	HRESULT Render_Model();
	HRESULT Render_Texture();


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;



private:
	PARTICLEDESC m_Desc;
	_float m_fLifeTimeAcc = 0.f;
	_float m_fStopValueAcc = 0.f;

	_float3 m_vRotDir;

	_bool m_bRotC = false;
	_float m_fRotationSpeed = 0.f;
	_float m_fAnigleAcc = 0.f;

private:
	HRESULT Ready_Components();

	HRESULT Ready_Model_Components();
	HRESULT Ready_Texture_Components();

public:
	static CParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END