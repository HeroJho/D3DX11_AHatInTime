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

class CMagic final : public CGameObject
{
public:
	enum STATE { STATE_IN, STATE_IDLE, STATE_GO, STATE_END };

public:
	typedef struct tagMagicDesc
	{
		_float3 vSentorPos;
		_float3 vStartPos;
		_float fGoTime;

	}MAGICDESC;

private:
	CMagic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMagic(const CMagic& rhs);
	virtual ~CMagic() = default;



public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


private:
	void Set_State(STATE eState);
	void Rotate_Sentor(_float fTimeDelta);

	void Tick_In(_float fTimeDelta);
	void Tick_Idle(_float fTimeDelta);
	void Tick_Go(_float fTimeDelta);



private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pParentTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;


private:
	STATE m_eState = STATE_END;


	_float3 m_vSentorPos;
	_float m_fGoTime = 0.f;
	_float m_fGoTimeAcc = 0.f;
	_float3 m_vGoDir;
	_float	m_fDirPow = 0.f;

	_float3 m_vRotationDir;
	_float m_fRotationSpeed = 0.f;

	_bool m_bOn = false;
	_bool m_bEx = false;

protected:
	HRESULT Ready_Components();


public:
	static CMagic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END