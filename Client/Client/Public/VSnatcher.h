#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CNavigation;
END

BEGIN(Client)

class CVSnatcher final : public CGameObject
{
public:
	enum STATE {STATE_IDLE, STATE_END };

private:
	CVSnatcher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVSnatcher(const CVSnatcher& rhs);
	virtual ~CVSnatcher() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	void Set_AnimLinearData(ANIM_LINEAR_DATA LinearData);
	void Set_State(STATE eState);
	void Set_Anim();





private:
	STATE			m_eState = STATE_END;
	STATE			m_ePreState = STATE_END;

	_uint			m_iIndex = 0;
	vector<_uint> m_iIndexs;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;


private:
	virtual HRESULT Ready_Components();


public:
	static CVSnatcher* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END