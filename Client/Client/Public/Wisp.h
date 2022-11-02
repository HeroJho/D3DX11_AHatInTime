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

class CWisp final : public CGameObject
{
public:
	typedef struct tagWispDesc
	{
		_float fRatio;
		_float3 vPos;
	}WISPDESC;

private:
	CWisp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWisp(const CWisp& rhs);
	virtual ~CWisp() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


public:
	void Set_Ratio(_uint iRatio) { m_fRatio = iRatio; }



private:
	HRESULT Ready_Components();



private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	_float	m_fRatio = 0.f;


public:
	static CWisp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END