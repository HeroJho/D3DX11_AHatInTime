#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CParticle final : public CGameObject
{
public:
	typedef struct tagPartileDesc
	{

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
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;


private:
	PARTICLEDESC m_Desc;
	_float m_fLifeTimeAcc = 0.f;
	_float m_fStopValueAcc = 0.f;

private:
	HRESULT Ready_Components();

public:
	static CParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END