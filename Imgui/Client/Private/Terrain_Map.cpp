#include "stdafx.h"
#include "..\Public\Terrain_Map.h"
#include "GameInstance.h"

#include "MapManager.h"

CTerrain_Map::CTerrain_Map(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain_Map::CTerrain_Map(const CTerrain_Map & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain_Map::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain_Map::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTerrain_Map::Tick(_float fTimeDelta)
{
	// m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
}

void CTerrain_Map::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



	if (m_pVIBufferCom->Picking(m_pTransformCom, &m_vMousePickPos))
	{
		if (pGameInstance->Mouse_Pressing(DIMK::DIMK_LBUTTON))
		{

			_float fHegith = CMapManager::Get_Instance()->Get_Height();
			_float fRad = CMapManager::Get_Instance()->Get_Rad();
			_float fSharp = CMapManager::Get_Instance()->Get_Sharp();

			m_pVIBufferCom->Make_Tick_Up(fHegith, fRad, fSharp, m_vMousePickPos, fTimeDelta);

		}
	}
	else
		ZeroMemory(&m_vMousePickPos, sizeof(_float3));



	RELEASE_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTerrain_Map::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	//ID3D11RasterizerState* m_WireFrame;
	//D3D11_RASTERIZER_DESC temp;
	//ZeroMemory(&temp, sizeof(D3D11_RASTERIZER_DESC));
	//temp.FillMode = D3D11_FILL_WIREFRAME;
	//temp.CullMode = D3D11_CULL_NONE;
	//temp.DepthClipEnable = true;
	//m_pDevice->CreateRasterizerState(&temp, &m_WireFrame);


	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	 //m_pDeviceContext->RSSetState(m_WireFrame);
	 //Safe_Release(m_WireFrame);

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}


HRESULT CTerrain_Map::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Terrain"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CVIBuffer_Map_Terrain::TERRAINDESC TerrainDesc;
	ZeroMemory(&TerrainDesc, sizeof(CVIBuffer_Map_Terrain::TERRAINDESC));
	TerrainDesc.iNumVerticesX = 200;
	TerrainDesc.iNumVerticesY = 200;


	_bool bColor = false;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bColor", &bColor, sizeof(_bool))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Map_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &TerrainDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CTerrain_Map::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_RawValue("g_vMousePosition", &m_vMousePickPos, sizeof(_float3))))
		return E_FAIL;
	_float fRad = CMapManager::Get_Instance()->Get_Rad();
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRad", &fRad, sizeof(_float))))
		return E_FAIL;
	

	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

CTerrain_Map * CTerrain_Map::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerrain_Map*		pInstance = new CTerrain_Map(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTerrain_Map"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain_Map::Clone(void * pArg)
{
	CTerrain_Map*		pInstance = new CTerrain_Map(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTerrain_Map"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain_Map::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
