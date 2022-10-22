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

class CRectBarrel final : public CGameObject
{

private:
	CRectBarrel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRectBarrel(const CRectBarrel& rhs);
	virtual ~CRectBarrel() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CGameObject* pOther) override;


private:
	HRESULT Ready_Components();

	class CGameObject* m_pOther = nullptr;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;


public:
	static CRectBarrel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END