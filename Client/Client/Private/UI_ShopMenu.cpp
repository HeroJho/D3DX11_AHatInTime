#include "stdafx.h"
#include "..\Public\UI_ShopMenu.h"
#include "GameInstance.h"


#include "ToolManager.h"


CUI_ShopMenu::CUI_ShopMenu(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUI_ShopMenu::CUI_ShopMenu(const CUI_ShopMenu & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_ShopMenu::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ShopMenu::Initialize(void * pArg)
{
	ZeroMemory(m_pText, sizeof(TCHAR) * MAX_PATH);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CUI_ShopMenu::Tick(_float fTimeDelta)
{


	// UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CUI_ShopMenu::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);
}

HRESULT CUI_ShopMenu::Render()
{
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



	CToolManager::Get_Instance()->Render_Fonts(TEXT("Font_Nexon"), m_pText, _float2(XMVectorGetX(Get_TotalPos()), XMVectorGetY(Get_TotalPos())), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), _float2(0.7f, 0.7f), true);



	// UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}





HRESULT CUI_ShopMenu::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopMenu_costbg"), TEXT("Com_Texture1"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}





void CUI_ShopMenu::Handle_Hover()
{
	int a = 0;
}

void CUI_ShopMenu::Handle_Down()
{
	int a = 0;
}

void CUI_ShopMenu::Handle_Press()
{
	int a = 0;
}

void CUI_ShopMenu::Handle_Click()
{

}



CUI_ShopMenu * CUI_ShopMenu::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_ShopMenu*		pInstance = new CUI_ShopMenu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_ShopMenu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_ShopMenu::Clone(void * pArg)
{
	CUI_ShopMenu*		pInstance = new CUI_ShopMenu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_ShopMenu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUI_ShopMenu::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
