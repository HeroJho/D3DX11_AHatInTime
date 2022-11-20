#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CLight;
END

BEGIN(Client)

class CFlaskLight : public CGameObject
{
public:
	typedef struct tagFlaskLightDesc
	{
		_float3 vPos;
	}FLASKLIGHTDESC;


protected:
	CFlaskLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFlaskLight(const CFlaskLight& rhs);
	virtual ~CFlaskLight() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


private:
	CTransform*				m_pTransformCom = nullptr;

	class CLight* m_pLight = nullptr;
	class CLight* m_pLight2 = nullptr;

	_float	m_fTimeAcc = 0.f;
	_float	m_fLightPower = 0.25f;

protected:
	HRESULT Ready_Components();

public:
	static CFlaskLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END