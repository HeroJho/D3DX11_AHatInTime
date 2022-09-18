#include "stdafx.h"
#include "..\Public\UI_Edit.h"
#include "GameInstance.h"

CUI_Edit::CUI_Edit(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_Panel(pDevice, pContext)
{
}

CUI_Edit::CUI_Edit(const CUI_Edit & rhs)
	: CUI_Panel(rhs)
{
}

HRESULT CUI_Edit::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Edit::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_UiInfo.fSizeX = 197;
	m_UiInfo.fSizeY = 380;

	m_UiInfo.fX = g_iWinSizeX * 0.5f;
	m_UiInfo.fY = g_iWinSizeY * 0.5f;


	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToMe(TEXT("Prototype_GameObject_UI_Button"), LEVEL_GAMEPLAY, &pObj)))
		return E_FAIL;
	m_pChildUIs.push_back((CUI*)pObj);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CUI_Edit::Tick(_float fTimeDelta)
{




	__super::Tick(fTimeDelta);
}

void CUI_Edit::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);
}

HRESULT CUI_Edit::Render()
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



	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}

HRESULT CUI_Edit::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenFrame"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CUI_Edit * CUI_Edit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Edit*		pInstance = new CUI_Edit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Edit::Clone(void * pArg)
{
	CUI_Edit*		pInstance = new CUI_Edit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Edit::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
