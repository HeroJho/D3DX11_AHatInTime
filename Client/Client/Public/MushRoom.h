#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CLight;
END

BEGIN(Client)

class CMushRoom final : public CGameObject
{
public:
	typedef struct tagMushRoomDesc
	{
		_float3 vPos;
		_float3 vRotation;
		_float3 vScale;

		_float4 vDiffuseColor;
		_float4 vAmColor;

		_float fPower;
		_float fUpSpeed;
		_float fDownSpeed;

		_uint	iIsLight;

	}MUSHROOMDESC;

public:
	enum STATE { STATE_DOWN_IDLE, STATE_DOWN_START, STATE_UP_START, STATE_UP_IDLE, STATE_END };

private:
	CMushRoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMushRoom(const CMushRoom& rhs);
	virtual ~CMushRoom() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	virtual HRESULT Render_ShadowDepth();

public:
	void Set_State(STATE eState);


private:
	void Tick_Down_Idle(_float fTimeDelta);
	void Tick_Down_Start(_float fTimeDelta);
	void Tick_Up_Start(_float fTimeDelta);
	void Tick_Up_Idle(_float fTimeDelta);

private:
	STATE m_eState = STATE_END;

	MUSHROOMDESC m_Desc;

	class CLight* m_pLight = nullptr;

	_float m_fRatio = 0.f;

	_float m_fTimeAcc = 0.f;
	_float m_fTime = 0.f;

	_bool m_bTurn = false;


private:
	HRESULT Ready_Components();


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;


public:
	static CMushRoom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END