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

class CToilet_Scream final : public CGameObject
{
public:
	typedef struct tagScreamDesc
	{
		_float3 vRotation;
	}SCREAMDESC;

private:
	CToilet_Scream(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToilet_Scream(const CToilet_Scream& rhs);
	virtual ~CToilet_Scream() = default;



public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	_float m_fTime = 0.f;


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTexture_Eye = nullptr;
	CTexture*				m_pTexture_Noise = nullptr;


private:
	HRESULT Ready_Components();

public:
	static CToilet_Scream* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
