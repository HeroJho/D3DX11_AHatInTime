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

class CTimeObject final : public CGameObject
{
public:
	typedef struct tagTimeObjectDesc
	{
		_float3 vPos;
		_float3 vRotation;
		_float3 vScale;
	}TIMEOBJECTDESC;

private:
	CTimeObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTimeObject(const CTimeObject& rhs);
	virtual ~CTimeObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;



private:
	virtual HRESULT Ready_Components();


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;



	_float					m_fDeadTimeAcc = 0.f;

public:
	static CTimeObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END