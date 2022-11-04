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

class CWisp final : public CGameObject
{
public:
	typedef struct tagWispDesc
	{
		CGameObject* pOwner = nullptr;
		_float fMaxRatio;
		_float fSpeed;
	}WISPDESC;

public:
	enum STATE { STATE_IDLE, STATE_RING_UP, STATE_RING_DOWN, STATE_END };

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

public:
	void Start();

private:
	HRESULT Ready_Components();


private:
	CGameObject* m_pOwner = nullptr;

	_float	m_fRatio = 0.f;
	_float m_fMaxRatio = 0.f;
	STATE m_eState = STATE_END;

	_float m_fSpeed = 0.f;

	
private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

public:
	static CWisp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END