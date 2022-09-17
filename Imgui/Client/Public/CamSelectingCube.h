#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_ColorCube;
END

BEGIN(Client)

class CCamSelectingCube final : public CGameObject
{
public:
	typedef struct tagColorCubeDesc
	{
		_float4 vColor;
	}COLORCUBEDESC;

private:
	CCamSelectingCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamSelectingCube(const CCamSelectingCube& rhs);
	virtual ~CCamSelectingCube() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float3 Get_Pos();
	void Set_Pos(_vector vPos);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_ColorCube*	m_pVIBufferCom = nullptr;

private:
	_float4 m_vRGB;

private:
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CCamSelectingCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END