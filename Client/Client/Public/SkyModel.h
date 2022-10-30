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

class CSkyModel final : public CGameObject
{
private:
	CSkyModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkyModel(const CSkyModel& rhs);
	virtual ~CSkyModel() = default;

public:
	_float3* Get_Axis() { return &m_vAxis; }


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


	_float3					m_vAxis;


private:
	HRESULT Ready_Components();

public:
	static CSkyModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
