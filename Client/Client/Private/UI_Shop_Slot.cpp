#include "stdafx.h"
#include "..\Public\UI_Shop_Slot.h"
#include "GameInstance.h"

#include "Player.h"

#include "ToolManager.h"
#include "ItemManager.h"
#include "UI_Shop_SlotItem.h"


CUI_Shop_Slot::CUI_Shop_Slot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Shop_Slot::CUI_Shop_Slot(const CUI_Shop_Slot & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Shop_Slot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Shop_Slot::Initialize(void * pArg)
{
	ZeroMemory(m_cItemTag, sizeof(MAX_PATH));


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == ((UIINFODESC*)pArg)->pDesc)
		return E_FAIL;

	SLOTDESC* pDesc = (SLOTDESC*)((UIINFODESC*)pArg)->pDesc;
	lstrcpy(m_cItemTag, pDesc->sItemTag);

	if (FAILED(Ready_Components()))
		return E_FAIL;


	CUI_Shop_SlotItem::SLOTITEMDESC SlotItemDesc;
	ZeroMemory(&SlotItemDesc, sizeof(CUI_Shop_SlotItem::SLOTITEMDESC));

	if (!lstrcmp(TEXT("Mask_Fox"), m_cItemTag))
	{
		lstrcpy(SlotItemDesc.sItemTag, TEXT("Prototype_Component_Texture_Foxmask"));
		Make_ChildUI(-4.f, -52.f, 90.f, 90.f, TEXT("Prototype_UI_Shop_SlotItem"), &SlotItemDesc);
	}
	else if (!lstrcmp(TEXT("Mask_Cat"), m_cItemTag))
	{
		lstrcpy(SlotItemDesc.sItemTag, TEXT("Prototype_Component_Texture_DwellerMask"));
		Make_ChildUI(-4.f, -52.f, 90.f, 90.f, TEXT("Prototype_UI_Shop_SlotItem"), &SlotItemDesc);
	}


	m_fOriSizeX = m_UiInfo.fSizeX;
	m_fOriSizeY = m_UiInfo.fSizeY;
	m_fHoverSizeX = m_UiInfo.fSizeX + 40.f;
	m_fHoverSizeY = m_UiInfo.fSizeY + 40.f;
	m_fHoverSpeed = 4.f;


	m_fOriMovePosX = m_UiInfo.fX;
	m_fOriMovePosY = m_UiInfo.fY;
	m_fHoverMovePosY = m_UiInfo.fY + 20.f;
	m_fMoveSpeed = 2.f;

	return S_OK;
}




void CUI_Shop_Slot::Select_Slot()
{
	m_eState = STATE_SELETED;
	if (!m_pChildUIs.empty())
		((CUI_Shop_SlotItem*)m_pChildUIs.front())->Select_Slot();
}

void CUI_Shop_Slot::UnSelect_Slot()
{
	m_eState = STATE_IDLE;
	if (!m_pChildUIs.empty())
		((CUI_Shop_SlotItem*)m_pChildUIs.front())->UnSelect_Slot();
}

void CUI_Shop_Slot::Start_BuyTick()
{
	m_eState = STATE_BUY;
}

void CUI_Shop_Slot::Buy_Item()
{
	Delete_AllChildUI();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
	RELEASE_INSTANCE(CGameInstance);

	pPlayer->Get_Hat(m_cItemTag);

	m_eState = STATE_IDLE;

	m_UiInfo.fX = m_fOriMovePosX;
	m_UiInfo.fY = m_fOriMovePosY;

	m_bIsBuyed = true;
}






void CUI_Shop_Slot::Tick(_float fTimeDelta)
{

	switch (m_eState)
	{
	case STATE_IDLE:
		Idle_Tick(fTimeDelta);
		break;
	case STATE_SELETED:
		Selected_Tick(fTimeDelta);
		break;
	case STATE_BUY:
		Buy_Tick(fTimeDelta);
		break;
	}


	// UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}


void CUI_Shop_Slot::Selected_Tick(_float fTimeDelta)
{
	if (m_fHoverSizeX > m_UiInfo.fSizeX)
	{
		m_UiInfo.fSizeX += m_fHoverSpeed;
		m_UiInfo.fSizeY += m_fHoverSpeed;
	}
	else
	{
		m_UiInfo.fSizeX = m_fHoverSizeX;
		m_UiInfo.fSizeY = m_fHoverSizeY;
	}

	if (m_fHoverMovePosY > m_UiInfo.fY)
		m_UiInfo.fY += m_fMoveSpeed;
	else
		m_UiInfo.fY = m_fHoverMovePosY;

}

void CUI_Shop_Slot::Idle_Tick(_float fTimeDelta)
{
	if (m_fHoverSizeX < m_UiInfo.fSizeX)
	{
		m_UiInfo.fSizeX -= m_fHoverSpeed;
		m_UiInfo.fSizeY -= m_fHoverSpeed;
	}
	else
	{
		m_UiInfo.fSizeX = m_fOriSizeX;
		m_UiInfo.fSizeY = m_fOriSizeY;
	}

	if (m_fHoverMovePosY < m_UiInfo.fY)
		m_UiInfo.fY -= m_fMoveSpeed;
	else
		m_UiInfo.fY = m_fOriMovePosY;


}

void CUI_Shop_Slot::Buy_Tick(_float fTimeDelta)
{
	if (1.f > m_UiInfo.fX && -1.f < m_UiInfo.fX)
	{
		m_UiInfo.fX = 0.f;
		return;
	}


	if (0.f > m_UiInfo.fX)
		m_UiInfo.fX += 200.f * fTimeDelta;
	else
		m_UiInfo.fX -= 200.f * fTimeDelta;

}










void CUI_Shop_Slot::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);
}

HRESULT CUI_Shop_Slot::Render()
{
	if (!m_bStopRend && STATE_BUY == m_eState)
	{
		__super::Render();
		return S_OK;
	}
	else if (m_bStopRend)
		return S_OK;


	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	_uint iTextureIndex = 0;
	if (STATE_SELETED == m_eState)
		iTextureIndex = 1;
	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	// UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}







HRESULT CUI_Shop_Slot::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_shop_stand"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}





void CUI_Shop_Slot::Handle_Hover()
{

}

void CUI_Shop_Slot::Handle_Down()
{
	int a = 0;
}

void CUI_Shop_Slot::Handle_Press()
{
	int a = 0;
}

void CUI_Shop_Slot::Handle_Click()
{

}



CUI_Shop_Slot * CUI_Shop_Slot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Shop_Slot*		pInstance = new CUI_Shop_Slot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Shop_Slot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Shop_Slot::Clone(void * pArg)
{
	CUI_Shop_Slot*		pInstance = new CUI_Shop_Slot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Shop_Slot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUI_Shop_Slot::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
