#include "stdafx.h"
#include "..\Public\UI.h"
#include "GameInstance.h"

CUI::CUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{
}





HRESULT CUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CUI::Tick(_float fTimeDelta)
{

	m_pTransformCom->Set_Scale(XMVectorSet(m_UiInfo.fSizeX, m_UiInfo.fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UiInfo.fX - g_iWinSizeX * 0.5f, -m_UiInfo.fY + g_iWinSizeY * 0.5f, 0.0f, 1.f));

	for (auto& pChildUI : m_pChildUIs)
		pChildUI->Tick(fTimeDelta);
}

void CUI::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	for (auto& pChildUI : m_pChildUIs)
		pChildUI->LateTick(fTimeDelta);
}

HRESULT CUI::Render()
{
	for (auto& pChildUI : m_pChildUIs)
		pChildUI->Render();

	return S_OK;
}














HRESULT CUI::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

CUI * CUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI*		pInstance = new CUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI::Clone(void * pArg)
{
	CUI*		pInstance = new CUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI::Free()
{
	__super::Free();

	for (auto& pChildUI : m_pChildUIs)
		Safe_Release(pChildUI);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
