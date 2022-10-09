#include "stdafx.h"
#include "..\Public\UI_TextLife.h"
#include "GameInstance.h"

CUI_TextLife::CUI_TextLife(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUI_TextLife::CUI_TextLife(const CUI_TextLife & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_TextLife::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_TextLife::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	// Make_ChildUI(100.f, 100.f, 32.f, 19.f, TEXT("Prototype_GameObject_UI_Edit_Button"));


	return S_OK;
}

void CUI_TextLife::Tick(_float fTimeDelta)
{




	__super::Tick(fTimeDelta);
}

void CUI_TextLife::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);
}

HRESULT CUI_TextLife::Render()
{
	// 자식들의 Render 호출
	__super::Render();

	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_TextLife::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LifeText"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CUI_TextLife * CUI_TextLife::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_TextLife*		pInstance = new CUI_TextLife(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_TextLife"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_TextLife::Clone(void * pArg)
{
	CUI_TextLife*		pInstance = new CUI_TextLife(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_TextLife"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_TextLife::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
