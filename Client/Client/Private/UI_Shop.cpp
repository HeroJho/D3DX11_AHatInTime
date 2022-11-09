#include "stdafx.h"
#include "..\Public\UI_Shop.h"
#include "GameInstance.h"


#include "ToolManager.h"

#include "UI_Shop_Slot.h"
#include "UI_ShopMenu.h"



CUI_Shop::CUI_Shop(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Shop::CUI_Shop(const CUI_Shop & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Shop::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Shop::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_State(STATE_CLOSEIDLE);

	CUI_Shop_Slot::SLOTDESC SlotDesc;
	ZeroMemory(&SlotDesc, sizeof(CUI_Shop_Slot::SLOTDESC));

	lstrcpy(SlotDesc.sItemTag, TEXT("Mask_Fox"));
	Make_ChildUI(-60.f, -20.f, 100.f, 35.f, TEXT("Prototype_UI_Shop_Slot"), &SlotDesc);
	lstrcpy(SlotDesc.sItemTag, TEXT("Mask_Cat"));
	Make_ChildUI(60.f, -20.f, 100.f, 35.f, TEXT("Prototype_UI_Shop_Slot"), &SlotDesc);

	Make_ChildUI(-1.66f, 74.22f, 372.44f, 87.f, TEXT("Prototype_UI_ShopMenu"));

	m_iSelectedIndex = 0;

	m_fOnOffSpeed = 3000.f;
	m_vOpenPos.x = m_UiInfo.fX;
	m_vOpenPos.y = m_UiInfo.fY;
	m_vClosePos.x = m_UiInfo.fX;
	m_vClosePos.y = 920.f;

	m_fDelayPosY = 150.f;
	m_fRatio = 1.f;
	m_bRatioStart = false;

	m_fAlpa = 1.f;

	return S_OK;
}

void CUI_Shop::Tick(_float fTimeDelta)
{


	switch (m_eState)
	{
	case Client::CUI_Shop::STATE_OPEN:
		Tick_Open(fTimeDelta);
		break;
	case Client::CUI_Shop::STATE_IDLE:
		Tick_Idle(fTimeDelta);
		break;
	case Client::CUI_Shop::STATE_BUY:
		Tick_Buy(fTimeDelta);
		break;
	case Client::CUI_Shop::STATE_CLOSE:
		Tick_Close(fTimeDelta);
		break;
	case Client::CUI_Shop::STATE_CLOSEIDLE:
		Tick_CloseIdle(fTimeDelta);
		break;
	}


	// UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CUI_Shop::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Shop::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	_bool bStart = false;
	if (STATE_BUY == m_eState)
		bStart = true;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Start", &bStart, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpa", &m_fAlpa, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pIdleTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pBuyTextureCom->Set_SRV(m_pShaderCom, "g_OtherDiffuseTexture", 0)))
		return E_FAIL;
	

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	// UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}

void CUI_Shop::Open()
{
	if (STATE_OPEN != m_eState)
		m_eState = STATE_OPEN;
}

void CUI_Shop::Close()
{
	if (STATE_CLOSE != m_eState)
		m_eState = STATE_CLOSE;
}




void CUI_Shop::Set_State(STATE eState)
{
	if (eState != m_eState)
	{
		switch (m_eState)
		{
		case Client::CUI_Shop::STATE_OPEN:
		{
			m_fRatio = 1.f;
			m_bDelayPosChange = false;
			m_bRatioStart = false;
		}
			break;
		case Client::CUI_Shop::STATE_IDLE:
			break;
		case Client::CUI_Shop::STATE_BUY:
			break;
		case Client::CUI_Shop::STATE_CLOSE:
			break;
		}
	}

	m_eState = eState;

}



void CUI_Shop::Tick_Idle(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_RIGHT))
	{
		++m_iSelectedIndex;
		if (1 < m_iSelectedIndex)
			m_iSelectedIndex = 1;
	}
	else if (pGameInstance->Key_Down(DIK_LEFT))
	{
		--m_iSelectedIndex;
		if (0 > m_iSelectedIndex)
			m_iSelectedIndex = 0;
	}
	else if (pGameInstance->Key_Down(DIK_SPACE))
	{
		_uint iIndex = 0;
		for (auto& pUI : m_pChildUIs)
		{
			if (1 < iIndex)
				break;
			CUI_Shop_Slot* pSlot = (CUI_Shop_Slot*)pUI;
		
			if (iIndex == m_iSelectedIndex && !pSlot->Get_IsBuyed())
				Set_State(STATE_BUY);

			++iIndex;
		}
	}

	RELEASE_INSTANCE(CGameInstance);



	_int iIndex = 0;
	_bool bIsBuyed_0 = false;
	_bool bIsBuyed_1 = false;
	for (auto& pUI : m_pChildUIs)
	{
		if (2 == iIndex)
		{
			CUI_ShopMenu* pMenu = (CUI_ShopMenu*)pUI;
			if(0 == m_iSelectedIndex && !bIsBuyed_0)
				pMenu->Set_Text(TEXT("시간은 공평하지 않습니다!"));
			else if (1 == m_iSelectedIndex && !bIsBuyed_1)
				pMenu->Set_Text(TEXT("새로운 장소를 찾아보세요!"));
			else
				pMenu->Set_Text(TEXT("빈 슬롯"));

			break;
		}
		else
		{
			CUI_Shop_Slot* pSlot = (CUI_Shop_Slot*)pUI;
			if (iIndex == m_iSelectedIndex)
				pSlot->Select_Slot();
			else
				pSlot->UnSelect_Slot();
			
			if(0 == iIndex)
				bIsBuyed_0 = pSlot->Get_IsBuyed();
			else if(1 == iIndex)
				bIsBuyed_1 = pSlot->Get_IsBuyed();

			++iIndex;
		}


	}

}

void CUI_Shop::Tick_Buy(_float fTimeDelta)
{
	m_fAlpa -= fTimeDelta * 10.f;

	
	if (0.f > m_fAlpa)
		m_fAlpa = 0.f;

	_int iIndex = 0;
	for (auto& pUI : m_pChildUIs)
	{
		if (1 < iIndex)
			break;

		CUI_Shop_Slot* pSlot = (CUI_Shop_Slot*)pUI;
		if (iIndex == m_iSelectedIndex)
			pSlot->Start_BuyTick();
		else
			pSlot->Set_StopRend(true);
		++iIndex;
	}



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		m_fAlpa = 1.f;
		Set_State(STATE_CLOSE);

		_int iIndex = 0;
		for (auto& pUI : m_pChildUIs)
		{
			if (1 < iIndex)
				break;

			CUI_Shop_Slot* pSlot = (CUI_Shop_Slot*)pUI;
			if (iIndex == m_iSelectedIndex)
				pSlot->Buy_Item();
			else
				pSlot->Set_StopRend(false);
			++iIndex;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CUI_Shop::Tick_Close(_float fTimeDelta)
{
	if (m_vClosePos.y < m_UiInfo.fY)
	{
		m_UiInfo.fY = m_vClosePos.y;
		Set_State(STATE_CLOSEIDLE);
	}
	else
		m_UiInfo.fY += m_fOnOffSpeed * fTimeDelta;
}

void CUI_Shop::Tick_Open(_float fTimeDelta)
{
	if (0.2f > m_fRatio && -0.2f < m_fRatio)
	{
		m_UiInfo.fY = m_vOpenPos.y;
		Set_State(STATE_IDLE);
		return;
	}

	if (m_bRatioStart)
		m_fRatio -= fTimeDelta * 5.f;

	if (m_vOpenPos.y - m_fDelayPosY * m_fRatio > m_UiInfo.fY && !m_bDelayPosChange)
	{
		m_bDelayPosChange = true;
		m_bRatioStart = true;
		m_UiInfo.fY = m_vOpenPos.y - m_fDelayPosY * m_fRatio;
	}
	else if (m_vOpenPos.y + m_fDelayPosY * m_fRatio < m_UiInfo.fY && m_bDelayPosChange)
	{
		m_bDelayPosChange = false;
		m_UiInfo.fY = m_vOpenPos.y + m_fDelayPosY * m_fRatio;
	}



	if (m_bDelayPosChange)
	{
		m_UiInfo.fY += m_fOnOffSpeed * fTimeDelta * m_fRatio;
	}
	else
	{
		m_UiInfo.fY -= m_fOnOffSpeed * fTimeDelta * m_fRatio;
	}

}

void CUI_Shop::Tick_CloseIdle(_float fTimeDelta)
{
	m_UiInfo.fY = m_vClosePos.y;
}




HRESULT CUI_Shop::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopWindow"), TEXT("Com_Texture1"), (CComponent**)&m_pIdleTextureCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopWindowItem"), TEXT("Com_Texture2"), (CComponent**)&m_pBuyTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}





void CUI_Shop::Handle_Hover()
{
	int a = 0;
}

void CUI_Shop::Handle_Down()
{
	int a = 0;
}

void CUI_Shop::Handle_Press()
{
	int a = 0;
}

void CUI_Shop::Handle_Click()
{

}



CUI_Shop * CUI_Shop::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Shop*		pInstance = new CUI_Shop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Shop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Shop::Clone(void * pArg)
{
	CUI_Shop*		pInstance = new CUI_Shop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Shop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUI_Shop::Free()
{
	__super::Free();

	Safe_Release(m_pIdleTextureCom);
	Safe_Release(m_pBuyTextureCom);
	Safe_Release(m_pShaderCom);
}
