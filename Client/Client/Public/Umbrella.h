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

class CUmbrella final : public CGameObject
{
private:
	CUmbrella(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUmbrella(const CUmbrella& rhs);
	virtual ~CUmbrella() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT SetUp_State(_fmatrix StateMatrix) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pParentTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

public:
	_float3* Get_Axis() { return &m_vAxis; }
	TCHAR* Get_ModelTag() { return m_cModelTag; }

private:
	TCHAR					m_cModelTag[MAX_PATH];
	_float3					m_vAxis;

	_float					m_fAttackTimeAcc = 0.f;
	_bool					m_bCanAttack = false;


private:
	HRESULT Ready_Components();

public:
	static CUmbrella* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END