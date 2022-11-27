#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Light_Manager.h"
#include "PipeLine.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	_uint		iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);


	// For.Target_ShadowDepth
	//_float		iShadowMapCX = ViewportDesc.Width * 6.25f;
	//_float		iShadowMapCY = ViewportDesc.Height * 6.25f;
	_float		iShadowMapCX = ViewportDesc.Width * 12.5f;
	_float		iShadowMapCY = ViewportDesc.Height * 12.5f;
	if (FAILED(m_pTarget_Manager->Ready_ShadowDepthStencilRenderTargetView(m_pDevice, iShadowMapCX, iShadowMapCY)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ShadowDepth"), iShadowMapCX, iShadowMapCY, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shader */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Blur */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Blur"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_BlurDownSample */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurDownSample"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_BlurHor */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurHor"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_BlurVer */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurVer"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_BlurUpSample */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurUpSample"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_Grow */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Grow"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* For.Target_Blend */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Blend"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;



	/* For.MRT_Deferred */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Blur"))))
		return E_FAIL;


	/* For.MRT_LightAcc */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;


	/* For.MRT_LightDepth*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightDepth"), TEXT("Target_ShadowDepth"))))
		return E_FAIL;


	/* For.MRT_BlurDownSample*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurDownSample"), TEXT("Target_BlurDownSample"))))
		return E_FAIL;
	/* For.MRT_BlurHor*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurHor"), TEXT("Target_BlurHor"))))
		return E_FAIL;
	/* For.MRT_BlurVer*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurVer"), TEXT("Target_BlurVer"))))
		return E_FAIL;
	/* For.MRT_BlurUpSample*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurUpSample"), TEXT("Target_BlurUpSample"))))
		return E_FAIL;
	/* For.MRT_Grow*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Grow"), TEXT("Target_Grow"))))
		return E_FAIL;
	

	/* For.MRT_Blend*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Blend"), TEXT("Target_Blend"))))
		return E_FAIL;


#ifdef _DEBUG

	//															렉트의 직교투영 욷르 만들기 위한 인자 값들
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Diffuse"), 100.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Blur"), 100.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_BlurDownSample"), 100.f, 500.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_BlurHor"), 300.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_BlurVer"), 300.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Grow"), 300.f, 500.f, 200.f, 200.f)))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Normal"), 100.f, 300.f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Depth"), 100.f, 500.f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Shade"), 300.f, 100.f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Specular"), 300.f, 300.f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_ShadowDepth"), 300.f, 500.f, 200.f, 200.f)))
	//	return E_FAIL;




	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));

#endif // _DEBUG


	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);


	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	// 광원 깊이를 그린다.
	if (FAILED(Render_ShadowDepth())) 
		return E_FAIL;

	// 디퍼드로 처리할 애들을 그린다. 
	if (FAILED(Render_NonAlphaBlend()))
		return E_FAIL;

	// 빛과 벡터로 빛을 그린다.
	if (FAILED(Render_Lights()))
		return E_FAIL;




	// 디퓨즈와 빛 연산을 한다.(안개 처리)
	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;


	// 블러
	if (FAILED(Render_BlurDownSample()))
		return E_FAIL;
	if (FAILED(Render_BlurHorizontal()))
		return E_FAIL;
	if (FAILED(Render_BlurVertical()))
		return E_FAIL;
	if (FAILED(Render_BlurUpSample()))
		return E_FAIL;
	if (FAILED(Render_Grow()))
		return E_FAIL;





	if (FAILED(Render_AlphaBlend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;


#ifdef _DEBUG


	if (FAILED(Render_Debug()))
		return E_FAIL;


#endif


	return S_OK;
}

HRESULT CRenderer::Add_DebugGroup(CComponent* pDebugCom)
{
	m_DebugObject.push_back(pDebugCom);

	Safe_AddRef(pDebugCom);

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_ShadowDepth()
{
	if (FAILED(m_pTarget_Manager->Begin_ShadowMRT(m_pContext, TEXT("MRT_LightDepth"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_SHADOWDEPTH])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_ShadowDepth();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_SHADOWDEPTH].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}


HRESULT CRenderer::Render_BlurDownSample()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Target_Shader타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BlurDownSample"))))
		return E_FAIL;


	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width * 0.5f, ViewportDesc.Height * 0.5f, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Blur"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;


	m_pShader->Begin(0);


	m_pVIBuffer->Render();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BlurHorizontal()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Target_Shader타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BlurHor"))))
		return E_FAIL;


	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_BlurDownSample"), m_pShader, "g_BlurTexture")))
		return E_FAIL;


	m_pShader->Begin(5);


	m_pVIBuffer->Render();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BlurVertical()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Target_Shader타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BlurVer"))))
		return E_FAIL;


	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_BlurHor"), m_pShader, "g_BlurTexture")))
		return E_FAIL;


	m_pShader->Begin(6);


	m_pVIBuffer->Render();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BlurUpSample()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Target_Shader타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BlurUpSample"))))
		return E_FAIL;


	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width * 2.f, ViewportDesc.Height * 2.f, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_BlurVer"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;


	m_pShader->Begin(0);


	m_pVIBuffer->Render();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Grow()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;


	//if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Grow"), false)))
	//	return E_FAIL;


	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Blend"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;;
	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_BlurUpSample"), m_pShader, "g_BlurTexture")))
		return E_FAIL;;


	m_pShader->Begin(7);


	m_pVIBuffer->Render();

	//if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
	//	return E_FAIL;

	return S_OK;
}


HRESULT CRenderer::Render_NonAlphaBlend()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONALPHABLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONALPHABLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Target_Shader타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;


	_float4x4			WorldMatrix;
	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;
	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);
	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;



	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);

	_float4x4			ViewMatrixInv;
	_float4x4			ProjMatrixInv;

	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(pPipeLine->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(pPipeLine->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ)));

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	m_pLight_Manager->Render(m_pShader, m_pVIBuffer);

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	// TODO Waring 코드 

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Blend"))))
		return E_FAIL;


	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return E_FAIL;



	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;



	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);
	// 그림자
	{
		if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_ShadowDepth"), m_pShader, "g_ShadowDepthTexture")))
			return E_FAIL;





		//_vector vPos = XMVectorSetW(XMLoadFloat3(&m_pTarget_Manager->Get_PlayerPos()), 1.f);
		//_vector		vLightAt = XMVectorSetW(vPos, 1.f);
		//vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 5.f);
		//vPos = XMVectorSetX(vPos, XMVectorGetX(vPos) - 7.f);
		//_vector		vLightEye = XMVectorSetW(vPos, 1.f);
		//_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 1.f);

		_float4x4		LightViewMatrix;
		if (FAILED(m_pShader->Set_RawValue("g_ShadowViewMatrix", &CLight_Manager::Get_Instance()->Get_ShadowLightViewMatrix(), sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShader->Set_RawValue("g_CamProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
			return E_FAIL;


	}


	// 안개를 위한 연산
	{
		if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4))))
			return E_FAIL;

	}

	RELEASE_INSTANCE(CPipeLine);

	// 윕스 안개 없애기
	{
		_bool bIsWisp = false;
		_int iWispNum = 0;
		_float* fRatios = nullptr;
		_float4* vPoss = nullptr;


		m_pTarget_Manager->Get_WispData(&fRatios, &vPoss, &bIsWisp, &iWispNum);

		if (FAILED(m_pShader->Set_RawValue("g_IsbWisp", &bIsWisp, sizeof(_bool))))
			return E_FAIL;

		if (bIsWisp)
		{
			if (FAILED(m_pShader->Set_RawValue("g_WispInfoNum", &iWispNum, sizeof(_int))))
				return E_FAIL;
			if (FAILED(m_pShader->Set_RawValue("g_WispRatios", fRatios, 256, 2)))
				return E_FAIL;
			if (FAILED(m_pShader->Set_RawValue("g_WispPoss", vPoss, 256, 3)))
				return E_FAIL;


			_float fMaxRatio = m_pTarget_Manager->Get_MaxWispRatio();
			_float fRatio = 1.f - (fRatios[0] / (fMaxRatio * 0.8f));
			_float fRendomNext = 0.f;
			if (0.f > fRatio)
				fRendomNext = 1.f;
			else
			{
				_float fShakeValue = 0.02f * fRatio;
				fRendomNext = m_pTarget_Manager->Get_RendomNum(1.f - fShakeValue, 1.f + fShakeValue);
			}

			if (FAILED(m_pShader->Set_RawValue("g_RendomNext", &fRendomNext, sizeof(_float))))
				return E_FAIL;
		}
	}


	if(m_pTarget_Manager->Get_Dark())
		m_pShader->Begin(4);
	else
		m_pShader->Begin(3);


	m_pVIBuffer->Render();


	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	m_RenderObjects[RENDER_ALPHABLEND].sort([](CGameObject* pSour, CGameObject* pDest)
	{
		return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	});

	for (auto& pRenderObject : m_RenderObjects[RENDER_ALPHABLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_ALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	m_pTarget_Manager->Render_Debug(TEXT("MRT_Deferred"), m_pVIBuffer, m_pShader);
	m_pTarget_Manager->Render_Debug(TEXT("MRT_LightAcc"), m_pVIBuffer, m_pShader);
	m_pTarget_Manager->Render_Debug(TEXT("MRT_LightDepth"), m_pVIBuffer, m_pShader);
	m_pTarget_Manager->Render_Debug(TEXT("MRT_BlurDownSample"), m_pVIBuffer, m_pShader);
	m_pTarget_Manager->Render_Debug(TEXT("MRT_BlurHor"), m_pVIBuffer, m_pShader);
	m_pTarget_Manager->Render_Debug(TEXT("MRT_BlurVer"), m_pVIBuffer, m_pShader);
	m_pTarget_Manager->Render_Debug(TEXT("MRT_BlurUpSample"), m_pVIBuffer, m_pShader);
	m_pTarget_Manager->Render_Debug(TEXT("MRT_Grow"), m_pVIBuffer, m_pShader);

	
	for (auto& pDebugCom : m_DebugObject)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}

	m_DebugObject.clear();

	return S_OK;
}
#endif // _DEBUG

CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer*			pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	for (auto& List : m_RenderObjects)
	{
		for (auto& pGameObject : List)
		{
			Safe_Release(pGameObject);
		}
		List.clear();
	}

#ifdef _DEBUG
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
#endif // _DEBUG

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
}
