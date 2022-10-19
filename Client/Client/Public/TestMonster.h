#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel_Instance;
class CNavigation;
END

BEGIN(Client)

class CTestMonster final : public CGameObject
{
public:
	enum MONSTER_STATE { MONSTER_IDLE, MONSTER_MOVE, MONSTER_CHASE, MONSTER_ATTACKED, MONSTER_DIE, MONSTER_END };

private:
	CTestMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestMonster(const CTestMonster& rhs);
	virtual ~CTestMonster() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	HRESULT Ready_Components();


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel_Instance*		m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

private:
	MONSTER_STATE			m_eState = MONSTER_IDLE;



public:
	static CTestMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END