#include "stdafx.h"
#include "..\Public\UI.h"
#include "GameInstance.h"

#include "ToolManager.h"

CUI::CUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_UiInfo, sizeof(UIINFODESC));
	ZeroMemory(&m_TotalInfo, sizeof(UIINFODESC));
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(&m_UiInfo, sizeof(UIINFODESC));
	ZeroMemory(&m_TotalInfo, sizeof(UIINFODESC));
}





_vector CUI::Get_TotalPos()
{
	_vector vPos = { m_UiInfo.pParent->Get_Info().fX + m_UiInfo.fX, m_UiInfo.pParent->Get_Info().fY + m_UiInfo.fY };

	return vPos;
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
		m_UiInfo.pParent = nullptr;

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
	// 내꺼 먼저
	if (nullptr != m_UiInfo.pParent)
	{
		m_TotalInfo.fX = m_UiInfo.pParent->Get_Info().fX + m_UiInfo.fX;
		m_TotalInfo.fY = m_UiInfo.pParent->Get_Info().fY + m_UiInfo.fY;
	}
	else
	{
		m_TotalInfo = m_UiInfo;
	}

	m_pTransformCom->Set_Scale(XMVectorSet(m_UiInfo.fSizeX, m_UiInfo.fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_TotalInfo.fX - g_iWinSizeX * 0.5f, -m_TotalInfo.fY + g_iWinSizeY * 0.5f, 0.0f, 1.f));


	// 자식들 ㄱㄱ
	for (auto& pChildUI : m_pChildUIs)
	{
		pChildUI->Tick(fTimeDelta);
	}

	Handle_Collision();
}

void CUI::LateTick(_float fTimeDelta)
{
	m_eState = UI_END;

	if (nullptr == m_pRendererCom)
		return;

	// m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	for (auto& pChildUI : m_pChildUIs)
		pChildUI->LateTick(fTimeDelta);



	// TODO: 콜리전 매니저 옮기기
	POINT		ptMouse;
	GetCursorPos(&ptMouse);

	ScreenToClient(g_hWnd, &ptMouse);

	RECT		rcUI;
	SetRect(&rcUI, m_TotalInfo.fX - m_UiInfo.fSizeX * 0.5f, m_TotalInfo.fY - m_UiInfo.fSizeY * 0.5f, m_TotalInfo.fX + m_UiInfo.fSizeX * 0.5f, m_TotalInfo.fY + m_UiInfo.fSizeY * 0.5f);

	if (PtInRect(&rcUI, ptMouse))
	{
		OnCollision(CCollider::OTHERTOMECOLDESC());
	}
}

HRESULT CUI::Render()
{
	for (auto& pChildUI : m_pChildUIs)
		pChildUI->Render();

	return S_OK;
}

HRESULT CUI::Make_ChildUI(_float fX, _float fY, _float fSizeX, _float fSizeY, _tchar * pTag, void* pArg)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));
	UiInfoDesc.pParent = this;
	UiInfoDesc.fSizeX = fSizeX;
	UiInfoDesc.fSizeY = fSizeY;
	UiInfoDesc.fX = fX;
	UiInfoDesc.fY = fY;
	UiInfoDesc.pDesc = pArg;

	CGameObject* pObj = nullptr;
	pObj = pGameInstance->Clone_GameObject(pTag, &UiInfoDesc);
	
	m_pChildUIs.push_back((CUI*)pObj);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CUI::Delete_AllChildUI()
{
	for (auto& pUI : m_pChildUIs)
	{
		pUI->Set_Dead(true);
		Safe_Release(pUI);
	}
	m_pChildUIs.clear();
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



void CUI::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
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

void CUI::UI_RenderDebug()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	string sTemp = to_string(m_UiInfo.fX);
	TCHAR cTemp[MAX_PATH];
	CToolManager::Get_Instance()->CtoTC(sTemp.data(), cTemp);
	pGameInstance->Render_Fonts(TEXT("Font_Nexon"), cTemp, _float2(5.f, 10.f));

	sTemp = to_string(m_UiInfo.fY);
	CToolManager::Get_Instance()->CtoTC(sTemp.data(), cTemp);
	pGameInstance->Render_Fonts(TEXT("Font_Nexon"), cTemp, _float2(5.f, 30.f));

	sTemp = to_string(m_UiInfo.fSizeX);
	CToolManager::Get_Instance()->CtoTC(sTemp.data(), cTemp);
	pGameInstance->Render_Fonts(TEXT("Font_Nexon"), cTemp, _float2(5.f, 50.f));

	sTemp = to_string(m_UiInfo.fSizeY);
	CToolManager::Get_Instance()->CtoTC(sTemp.data(), cTemp);
	pGameInstance->Render_Fonts(TEXT("Font_Nexon"), cTemp, _float2(5.f, 70.f));

	RELEASE_INSTANCE(CGameInstance);
}

void CUI::UI_InputDebug(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(DIK_UP))
	{
		m_UiInfo.fY -= 50.f * fTimeDelta;
	}
	else if (pGameInstance->Key_Pressing(DIK_DOWN))
	{
		m_UiInfo.fY += 50.f  * fTimeDelta;
	}
	else if (pGameInstance->Key_Pressing(DIK_LEFT))
	{
		m_UiInfo.fX -= 50.f  * fTimeDelta;
	}
	else if (pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_UiInfo.fX += 50.f  * fTimeDelta;
	}
	else if (pGameInstance->Key_Pressing(DIK_X))
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
			m_UiInfo.fSizeX -= 50.f  * fTimeDelta;
		else
			m_UiInfo.fSizeX += 50.f  * fTimeDelta;
	}
	else if (pGameInstance->Key_Pressing(DIK_Y))
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
			m_UiInfo.fSizeY -= 50.f  * fTimeDelta;
		else
			m_UiInfo.fSizeY += 50.f  * fTimeDelta;
	}
	else if (pGameInstance->Key_Pressing(DIK_U))
	{
		m_UiInfo.fSizeX += 50.f  * fTimeDelta;
		m_UiInfo.fSizeY += 50.f  * fTimeDelta;
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		m_UiInfo.fSizeX -= 50.f  * fTimeDelta;
		m_UiInfo.fSizeY -= 50.f  * fTimeDelta;
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
