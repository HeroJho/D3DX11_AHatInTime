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

class CPuzzleCube_Boss : public CGameObject
{
public:
	typedef struct tagPuzzleCubeDesc
	{
		_float3 vPos;

	}PUZZLECUBEDESC;

	enum STATE { STATE_OUT, STATE_IN, STATE_END };

private:
	CPuzzleCube_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPuzzleCube_Boss(const CPuzzleCube_Boss& rhs);
	virtual ~CPuzzleCube_Boss() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	void Tick_In(_float fTimeDelta);
	void Tick_Out(_float fTimeDelta);

	void Attacked();

private:
	virtual HRESULT Ready_Components();


private:
	_float					m_fAlpa = 0.f;
	STATE					m_eState = STATE_END;

	_float					m_fUpOnTimeAcc = 0.f;
	_float					m_bInitDrop = false;

	_bool					m_bOn = false;
	_bool					m_bPreOn = false;
	_bool					m_Pushed = false;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

	CModel*					m_pIceModelCom = nullptr;
	CModel*					m_pWoodModelCom = nullptr;


public:
	static CPuzzleCube_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END