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

class CFlask_EX : public CGameObject
{
public:
	typedef struct tagFlaskExDesc
	{
		_float3 vPos;
	}FLASKEXDESC;


protected:
	CFlask_EX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFlask_EX(const CFlask_EX& rhs);
	virtual ~CFlask_EX() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


protected:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

	_bool					m_bTickAttack = false;

protected:
	HRESULT Ready_Components();

public:
	static CFlask_EX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END