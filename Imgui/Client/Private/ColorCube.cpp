#include "stdafx.h"
#include "..\Public\ColorCube.h"

#include "GameInstance.h"
#include "CamManager.h"

CColorCube::CColorCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CColorCube::CColorCube(const CColorCube & rhs)
	: CGameObject(rhs)
{
}


HRESULT CColorCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CColorCube::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	COLORCUBEDESC* pDesc = (COLORCUBEDESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&(pDesc->vPos)));
	m_pTransformCom->Set_Scale(XMLoadFloat3(&(pDesc->vScale)));

	m_vRGB.x = pDesc->iR;
	m_vRGB.y = pDesc->iG;
	m_vRGB.z = pDesc->iB;

	m_pVIBufferCom->SetColor(m_vRGB.x, m_vRGB.y, m_vRGB.z);

	

	return S_OK;
}

void CColorCube::Tick(_float fTimeDelta)
{

}

void CColorCube::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);



}

HRESULT CColorCube::Render()
{
	if (CCamManager::Get_Instance()->Get_SelectedMarkCubeTag() == m_sTag)
		m_pVIBufferCom->SetColor(255, 212, 0);
	else
		m_pVIBufferCom->SetColor(m_vRGB.x, m_vRGB.y, m_vRGB.z);


	if (FAILED(m_pTransformCom->Bind_WorldMatrix()))
		return E_FAIL;

	m_pGraphic_Device->SetTexture(0, nullptr);

	if (FAILED(Set_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

void CColorCube::Set_Pos(_float3 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

_float3 CColorCube::Get_Pos()
{
	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}


HRESULT CColorCube::Set_RenderState()
{
	if (nullptr == m_pDevice || nullptr == m_pDeviceContext)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}

HRESULT CColorCube::Reset_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}

HRESULT CColorCube::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_ColorCube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
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

CColorCube * CColorCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CColorCube*		pInstance = new CColorCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CColorCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CColorCube::Clone(void* pArg)
{
	CColorCube*		pInstance = new CColorCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CColorCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CColorCube::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

