#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel_Instance;
END

BEGIN(Client)

class CStaticModel_Instance final : public CGameObject
{
public:
	typedef struct tagStaticModelDesc
	{
		TCHAR		cModelTag[MAX_PATH];
		_uint		iNumInstance;
		VTXINSTANCE* pLocalInfos;

	}STATICMODELDESC;

private:
	CStaticModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaticModel_Instance(const CStaticModel_Instance& rhs);
	virtual ~CStaticModel_Instance() = default;



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
	CModel_Instance*		m_pModelCom = nullptr;

private:
	TCHAR					m_cModelTag[MAX_PATH];
	_float3					m_vAxis;

private:
	HRESULT Ready_Components();

public:
	static CStaticModel_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END