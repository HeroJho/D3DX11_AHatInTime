#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CRotateBarrel final : public CGameObject
{
public:
	typedef struct tagRotateBarrelDesc
	{
		_float3 vPos;
		_float3 vRotation;


		_float fSpeed;

	}ROTATEBARRELDESC;

private:
	CRotateBarrel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRotateBarrel(const CRotateBarrel& rhs);
	virtual ~CRotateBarrel() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


private:
	HRESULT Ready_Components();

	class CGameObject* m_pOther = nullptr;


private:
	ROTATEBARRELDESC m_Desc;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;


public:
	static CRotateBarrel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END