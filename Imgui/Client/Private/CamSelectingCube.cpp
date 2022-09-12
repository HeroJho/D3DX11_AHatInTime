#include "stdafx.h"
#include "..\Public\CamSelectingCube.h"

#include "GameInstance.h"

CCamSelectingCube::CCamSelectingCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCamSelectingCube::CCamSelectingCube(const CCamSelectingCube & rhs)
	: CGameObject(rhs)
{
}


HRESULT CCamSelectingCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamSelectingCube::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	COLORCUBEDESC* pDesc = (COLORCUBEDESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3{ 0.f, 0.f, 0.f });

	m_vRGB.x = pDesc->iR;
	m_vRGB.y = pDesc->iG;
	m_vRGB.z = pDesc->iB;

	m_pVIBufferCom->SetColor(m_vRGB.x, m_vRGB.y, m_vRGB.z);

	

	return S_OK;
}

void CCamSelectingCube::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);





	if (pGameInstance->Key_Pressing(DIK_UP))
		m_pTransformCom->Go_Dir(_float3{ 0.f, 0.f, 1.f }, 10.f, fTimeDelta);
	else if (pGameInstance->Key_Pressing(DIK_DOWN))
		m_pTransformCom->Go_Dir(_float3{ 0.f, 0.f, -1.f }, 10.f, fTimeDelta);
	else if (pGameInstance->Key_Pressing(DIK_LEFT))
		m_pTransformCom->Go_Dir(_float3{ -1.f, 0.f, 0.f }, 10.f, fTimeDelta);
	else if (pGameInstance->Key_Pressing(DIK_RIGHT))
		m_pTransformCom->Go_Dir(_float3{ 1.f, 0.f, 0.f }, 10.f, fTimeDelta);
	else if (pGameInstance->Key_Pressing(DIK_U))
		m_pTransformCom->Go_Dir(_float3{ 0.f, 1.f, 0.f }, 10.f, fTimeDelta);
	else if (pGameInstance->Key_Pressing(DIK_J))
		m_pTransformCom->Go_Dir(_float3{ 0.f, -1.f, 0.f }, 10.f, fTimeDelta);





	Safe_Release(pGameInstance);
}

void CCamSelectingCube::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CCamSelectingCube::Render()
{
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




_float3 CCamSelectingCube::Get_Pos()
{
	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}
void CCamSelectingCube::Set_Pos(_float3 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}





HRESULT CCamSelectingCube::Set_RenderState()
{
	if (nullptr == m_pDevice || nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}

HRESULT CCamSelectingCube::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}

HRESULT CCamSelectingCube::SetUp_Components()
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

CCamSelectingCube * CCamSelectingCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamSelectingCube*		pInstance = new CCamSelectingCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamSelectingCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamSelectingCube::Clone(void* pArg)
{
	CCamSelectingCube*		pInstance = new CCamSelectingCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCamSelectingCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamSelectingCube::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

