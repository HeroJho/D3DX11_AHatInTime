#include "stdafx.h"
#include "..\Public\UI_Health.h"
#include "GameInstance.h"

CUI_Health::CUI_Health(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Health::CUI_Health(const CUI_Health & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Health::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Health::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	 Make_ChildUI(-80.f, -50.f, 100.f, 100.f, TEXT("Prototype_UI_TextLife"));


	 m_vOnPos.x = m_UiInfo.fX;
	 m_vOnPos.y = m_UiInfo.fY;

	 m_vOffPos.x = m_UiInfo.fX;
	 m_vOffPos.y = m_UiInfo.fY - 300.f;

	 m_bIsOn = false;
	 m_UiInfo.fY = m_vOffPos.y;


	return S_OK;
}

void CUI_Health::Tick(_float fTimeDelta)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_M))
	{
		if (m_bIsOn)
			m_bIsOn = false;
		else
			m_bIsOn = true;
	}

	if (!m_bIsOn)
	{
		m_UiInfo.fY -= 300.f * fTimeDelta;

		if (m_UiInfo.fY < m_vOffPos.y)
			m_UiInfo.fY = m_vOffPos.y;
	}
	else
	{
		m_UiInfo.fY += 300.f * fTimeDelta;

		if (m_UiInfo.fY > m_vOnPos.y)
			m_UiInfo.fY = m_vOnPos.y;
	}

	RELEASE_INSTANCE(CGameInstance);


	__super::Tick(fTimeDelta);
}

void CUI_Health::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Health::Render()
{
	// 자식들의 Render 호출
	__super::Render();

	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 3)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Health::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerHealth"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CUI_Health * CUI_Health::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Health*		pInstance = new CUI_Health(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Health"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Health::Clone(void * pArg)
{
	CUI_Health*		pInstance = new CUI_Health(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Health"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Health::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
