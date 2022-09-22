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

	if (nullptr != pArg)
	{
		m_UiInfo = *((UIINFODESC*)pArg);
	}
	else
	{
		m_UiInfo.fSizeX = g_iWinSizeX;
		m_UiInfo.fSizeY = g_iWinSizeY;

		m_UiInfo.fX = g_iWinSizeX * 0.5f;
		m_UiInfo.fY = g_iWinSizeY * 0.5f;
	}
	

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));


	return S_OK;
}

void CUI::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_Scale(XMVectorSet(m_UiInfo.fSizeX, m_UiInfo.fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UiInfo.fX - g_iWinSizeX * 0.5f, -m_UiInfo.fY + g_iWinSizeY * 0.5f, 0.0f, 1.f));

	for (auto& pChildUI : m_pChildUIs)
		pChildUI->Tick(fTimeDelta);

	Handle_Collision();
}

void CUI::LateTick(_float fTimeDelta)
{
	m_eState = UI_END;

	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	for (auto& pChildUI : m_pChildUIs)
		pChildUI->LateTick(fTimeDelta);



	// TODO: 콜리전 매니저 옮기기
	POINT		ptMouse;
	GetCursorPos(&ptMouse);

	ScreenToClient(g_hWnd, &ptMouse);

	RECT		rcUI;
	SetRect(&rcUI, m_UiInfo.fX - m_UiInfo.fSizeX * 0.5f, m_UiInfo.fY - m_UiInfo.fSizeY * 0.5f, m_UiInfo.fX + m_UiInfo.fSizeX * 0.5f, m_UiInfo.fY + m_UiInfo.fSizeY * 0.5f);

	if (PtInRect(&rcUI, ptMouse))
	{
		OnCollision(nullptr);
	}
}

HRESULT CUI::Render()
{
	for (auto& pChildUI : m_pChildUIs)
		pChildUI->Render();

	return S_OK;
}

HRESULT CUI::Make_ChildUI(_float fX, _float fY, _float fSizeX, _float fSizeY, _tchar * pTag)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));
	UiInfoDesc.fSizeX = fSizeX;
	UiInfoDesc.fSizeY = fSizeY;
	UiInfoDesc.fX = fX;
	UiInfoDesc.fY = fY;

	CGameObject* pObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToMe(pTag, LEVEL_GAMEPLAY, &pObj, &UiInfoDesc)))
		return E_FAIL;
	m_pChildUIs.push_back((CUI*)pObj);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}




void CUI::Handle_Collision()
{
	switch (m_eState)
	{
	case UI_HOVER:
		Handle_Hover();
		break;
	case UI_DOWN:
		Handle_Down();
		break;
	case UI_PRESS:
		Handle_Press();
		break;
	case UI_CLICK:
		Handle_Click();
		break;
	}
}



void CUI::OnCollision(CGameObject * pOther)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (pGameInstance->Mouse_Down(DIMK_LBUTTON))
	{
		m_eState = UI_DOWN;
	}
	else if (pGameInstance->Mouse_Pressing(DIMK_LBUTTON))
	{
		m_eState = UI_PRESS;
	}
	else if (pGameInstance->Mouse_Up(DIMK_LBUTTON))
	{
		m_eState = UI_CLICK;
	}
	else
	{
		m_eState = UI_HOVER;
	}


	RELEASE_INSTANCE(CGameInstance);
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
