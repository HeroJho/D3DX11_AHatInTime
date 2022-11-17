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

class CSwipsSky final : public CGameObject
{
public:
	typedef struct tagSwipsSkyDesc
	{
		CGameObject* pOwner = nullptr;
	}SWIPSSKYDESC;

private:
	CSwipsSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwipsSky(const CSwipsSky& rhs);
	virtual ~CSwipsSky() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


private:
	HRESULT Ready_Components();


private:
	CGameObject* m_pOwner = nullptr;


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;


public:
	static CSwipsSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END