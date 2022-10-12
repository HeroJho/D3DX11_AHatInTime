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

class CClickedVertexCube final : public CGameObject
{
public:
	typedef struct tagColorCubeDesc
	{
		_float4 vColor;
		_float3 vPos;
		_float3 vScale;
	}COLORCUBEDESC;

private:
	CClickedVertexCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CClickedVertexCube(const CClickedVertexCube& rhs);
	virtual ~CClickedVertexCube() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Pos(_float3 vPos);
	_float3 Get_Pos();


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_ColorCube*	m_pVIBufferCom = nullptr;

	_float4			m_vRGB;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();


public:
	static CClickedVertexCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END