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

class CWitchChargEffect final : public CGameObject
{
private:
	CWitchChargEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWitchChargEffect(const CWitchChargEffect& rhs);
	virtual ~CWitchChargEffect() = default;

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
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;


private:
	class CLight*		m_pLight = nullptr;
	class CPlayer*		m_pPlayer = nullptr;

	_float		m_fTimeAcc = 0.f;
	_float		m_fAngle = 0.f;
	_float		m_fRatio = 0.f;
	_float		m_fRatioAcc = 1.f;

public:
	static CWitchChargEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END