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

class CIceBox : public CGameObject
{
public:
	typedef struct tagIceBoxDesc
	{
		_float3 vPos;
		_float3 vScale;
		CGameObject* pOwner;
	}ICEBOXDESC;

private:
	CIceBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIceBox(const CIceBox& rhs);
	virtual ~CIceBox() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	void Attacked();

private:
	virtual HRESULT Ready_Components();


private:
	class CBadgeS_Base* m_pOwner = nullptr;


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

	CModel*					m_pIceModelCom = nullptr;


public:
	static CIceBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END