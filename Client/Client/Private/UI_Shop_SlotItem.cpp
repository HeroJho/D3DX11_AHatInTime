#include "stdafx.h"
#include "..\Public\UI_Shop_SlotItem.h"
#include "GameInstance.h"


#include "ToolManager.h"


CUI_Shop_SlotItem::CUI_Shop_SlotItem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Shop_SlotItem::CUI_Shop_SlotItem(const CUI_Shop_SlotItem & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Shop_SlotItem::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Shop_SlotItem::Initialize(void * pArg)
{
	ZeroMemory(m_cItemTag, sizeof(MAX_PATH));


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == ((UIINFODESC*)pArg)->pDesc)
		return E_FAIL;

	SLOTITEMDESC* pDesc = (SLOTITEMDESC*)((UIINFODESC*)pArg)->pDesc;
	memcpy(m_cItemTag, pDesc->sItemTag, sizeof(TCHAR) * MAX_PATH);

	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_fOriSizeX = m_UiInfo.fSizeX;
	m_fOriSizeY = m_UiInfo.fSizeY;
	m_fHoverSizeX = m_UiInfo.fSizeX + 40.f;
	m_fHoverSizeY = m_UiInfo.fSizeY + 40.f;
	m_fHoverSpeed = 4.f;

	m_fOriMovePosY = m_UiInfo.fY;
	m_fHoverMovePosY = m_UiInfo.fY - 20.f;
	m_fMoveSpeed = 2.f;

	m_fIdleMaxSizeX = m_UiInfo.fSizeX + 50.f;
	m_fIdleMaxSizeY = m_UiInfo.fSizeY + 50.f;
	m_fIdleSpeed = 0.3f;

	return S_OK;
}



void CUI_Shop_SlotItem::Select_Slot()
{
	if (STATE_SELETED == m_eState)
		return;

	m_eState = STATE_SELETED;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->PlaySoundW(L"Hover Over Item.mp3", SOUND_UI, g_fEffectSound + 0.2f);
	RELEASE_INSTANCE(CGameInstance);
}

void CUI_Shop_SlotItem::UnSelect_Slot()
{
	m_eState = STATE_IDLE;
	m_bIdleStart = false;
	m_bIdleChange = false;
}






void CUI_Shop_SlotItem::Tick(_float fTimeDelta)
{

	switch (m_eState)
	{
	case STATE_IDLE:
		Idle_Tick(fTimeDelta);
		break;
	case STATE_SELETED:
		Selected_Tick(fTimeDelta);
		break;
	}


	// UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}


void CUI_Shop_SlotItem::Selected_Tick(_float fTimeDelta)
{


	if (!m_bIdleStart)
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
			m_bIdleStart = true;
		}

	}
	else
	{
		if (m_fIdleMaxSizeX < m_UiInfo.fSizeX)
		{
			m_bIdleChange = true;
			m_UiInfo.fSizeX = m_fIdleMaxSizeX;
			m_UiInfo.fSizeY = m_fIdleMaxSizeY;
		}
		if (m_fHoverSizeX > m_UiInfo.fSizeX)
		{
			m_bIdleChange = false;
			m_UiInfo.fSizeX = m_fHoverSizeX;
			m_UiInfo.fSizeY = m_fHoverSizeY;
		}


		if (m_bIdleChange)
		{
			m_UiInfo.fSizeX -= m_fIdleSpeed;
			m_UiInfo.fSizeY -= m_fIdleSpeed;
		}
		else
		{
			m_UiInfo.fSizeX += m_fIdleSpeed;
			m_UiInfo.fSizeY += m_fIdleSpeed;
		}
	}






	if (m_fHoverMovePosY < m_UiInfo.fY)
		m_UiInfo.fY -= m_fMoveSpeed;
	else
		m_UiInfo.fY = m_fHoverMovePosY;


}

void CUI_Shop_SlotItem::Idle_Tick(_float fTimeDelta)
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

	if (m_fHoverMovePosY > m_UiInfo.fY)
		m_UiInfo.fY += m_fMoveSpeed;
	else
		m_UiInfo.fY = m_fOriMovePosY;
}










void CUI_Shop_SlotItem::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);
}

HRESULT CUI_Shop_SlotItem::Render()
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

	// UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}







HRESULT CUI_Shop_SlotItem::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_cItemTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}





void CUI_Shop_SlotItem::Handle_Hover()
{

}

void CUI_Shop_SlotItem::Handle_Down()
{
	int a = 0;
}

void CUI_Shop_SlotItem::Handle_Press()
{
	int a = 0;
}

void CUI_Shop_SlotItem::Handle_Click()
{

}



CUI_Shop_SlotItem * CUI_Shop_SlotItem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Shop_SlotItem*		pInstance = new CUI_Shop_SlotItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Shop_SlotItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Shop_SlotItem::Clone(void * pArg)
{
	CUI_Shop_SlotItem*		pInstance = new CUI_Shop_SlotItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Shop_SlotItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUI_Shop_SlotItem::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
