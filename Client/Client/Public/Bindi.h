#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CTexture;
class CVIBuffer_Rect;
class CLight;
END

BEGIN(Client)

class CBindi final : public CGameObject
{
public:
	enum STATE { STATE_DOWN, STATE_HOVER, STATE_UP, STATE_END };

public:
	typedef struct tagBindiDesc
	{
		_float3 vPos;
	}BINDIDESC;

private:
	CBindi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBindi(const CBindi& rhs);
	virtual ~CBindi() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


public:
	void Set_State(STATE eState);


private:
	void Tick_Down(_float fTimeDelta);
	void Tick_UP(_float fTimeDelta);
	void Tick_Hover(_float fTimeDelta);

	void Comput_Rand(_float fTimeDelta);
	void Comput_BasiPos();

private:
	HRESULT Ready_Components();

private:
	STATE m_eState = STATE_END;
	_float m_fOriScale = 0.f;

	_float m_fScaleTimeAcc = 0.f;
	_bool m_bScaleTurn = false;


	_float m_fRanMinScale = 0.f;
	_float m_fRanSpeed = 0.f; // 어느 스피드로
	// _float m_fAngle = 0.f; // 몇 도 꺾을건지

	_float m_fRandTime = 0.f;
	_float m_fRandTimeAcc = 0.f;

	_float4 m_vDiffu;
	_float4 m_vAmbi;

	// 이동
	_float3 m_vOriPos;
	_float3 m_vMovePos[4];
	_float m_fMoveMaxRange = 0.f;
	_float m_fBasiT = 0.f;
	_float3 m_vTargetPos;
	_float m_fMoveSpeed = 0.f;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;


private:
	class CLight*		m_pLight = nullptr;


public:
	static CBindi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END