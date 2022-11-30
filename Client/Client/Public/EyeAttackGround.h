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

class CEyeAttackGround final : public CGameObject
{
public:
	typedef struct tagEyeAttackGroundDesc
	{
		_float3 vPos;
	}EYETTACKGROUNDDESC;

private:
	CEyeAttackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEyeAttackGround(const CEyeAttackGround& rhs);
	virtual ~CEyeAttackGround() = default;


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
	CTexture*				m_pTextureCom0 = nullptr;
	CTexture*				m_pTextureCom1 = nullptr;

private:
	_float				m_fDeltaTime = 0.f;

	_float				m_fScaleSpeed = 0.5f;
	_float3				m_vOriScale;


private:


private:
	HRESULT Ready_Components();


public:
	static CEyeAttackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END