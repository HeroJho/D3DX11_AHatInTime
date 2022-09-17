#include "stdafx.h"
#include "..\Public\MarkCube.h"

#include "GameInstance.h"
#include "CamManager.h"

CMarkCube::CMarkCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMarkCube::CMarkCube(const CMarkCube & rhs)
	: CGameObject(rhs)
{
}


HRESULT CMarkCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMarkCube::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	COLORCUBEDESC* pDesc = (COLORCUBEDESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 0.f, 0.f, 0.f, 1.f });

	_vector vTempPos = XMLoadFloat3(&(pDesc->vPos));
	vTempPos = XMVectorSetW(vTempPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTempPos);
	m_pTransformCom->Set_Scale(XMLoadFloat3(&(pDesc->vScale)));

	m_vRGB = pDesc->vColor;

	return S_OK;
}

void CMarkCube::Tick(_float fTimeDelta)
{

}

void CMarkCube::LateTick(_float fTimeDelta)
{

	if (CCamManager::Get_Instance()->Get_ShowCube())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMarkCube::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CMarkCube::Set_Pos(_float3 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
}

_float3 CMarkCube::Get_Pos()
{
	_float3 vTempPos;
	ZeroMemory(&vTempPos, sizeof(_float3));
	XMStoreFloat3(&vTempPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return vTempPos;
}


HRESULT CMarkCube::Set_RenderState()
{
	if (nullptr == m_pDevice || nullptr == m_pDeviceContext)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}

HRESULT CMarkCube::Reset_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}

HRESULT CMarkCube::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_ColorCube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For. Com_Shader*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_ColorCube"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(TransformDesc));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMarkCube::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);




	_float4 vTempColor;
	ZeroMemory(&vTempColor, sizeof(_float4));
	if (CCamManager::Get_Instance()->Get_SelectedMarkCubeTag() == m_sTag)
	{
		vTempColor.x = 1.f;
		vTempColor.y = 1.f;
		vTempColor.z = 0.f;
		vTempColor.w = 1.f;
	}
	else
	{
		vTempColor = m_vRGB;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &vTempColor, sizeof(_float4))))
		return E_FAIL;


	return S_OK;
}

CMarkCube * CMarkCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMarkCube*		pInstance = new CMarkCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMarkCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMarkCube::Clone(void* pArg)
{
	CMarkCube*		pInstance = new CMarkCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMarkCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMarkCube::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

