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

class CTest final : public CGameObject
{
private:
	CTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTest(const CTest& rhs);
	virtual ~CTest() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

public:
	_float3* Get_Axis() { return &m_vAxis; }
	TCHAR* Get_ModelTag() { return m_cModelTag; }

private:
	TCHAR					m_cModelTag[MAX_PATH];
	_float3					m_vAxis;

private:
	HRESULT Ready_Components();

public:
	static CTest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END