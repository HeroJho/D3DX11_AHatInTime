#include "stdafx.h"
#include "..\Public\UI_Button.h"
#include "GameInstance.h"

CUI_Button::CUI_Button(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_Panel(pDevice, pContext)
{
}

CUI_Button::CUI_Button(const CUI_Button & rhs)
	: CUI_Panel(rhs)
{
}

HRESULT CUI_Button::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Button::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_UiInfo.fSizeX = 29;
	m_UiInfo.fSizeY = 17;

	m_UiInfo.fX = 100.f;
	m_UiInfo.fY = 100.f;


	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));

	return S_OK;
}

void CUI_Button::Tick(_float fTimeDelta)
{




	__super::Tick(fTimeDelta);
}

void CUI_Button::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);
}

HRESULT CUI_Button::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}

HRESULT CUI_Button::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenBtn"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CUI_Button * CUI_Button::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Button*		pInstance = new CUI_Button(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Button"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Button::Clone(void * pArg)
{
	CUI_Button*		pInstance = new CUI_Button(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Button"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Button::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
