#include "stdafx.h"
#include "..\Public\UI_Item_Inven_Slot.h"
#include "GameInstance.h"


#include "ToolManager.h"

#include "Player.h"

CUI_Item_Inven_Slot::CUI_Item_Inven_Slot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Item_Inven_Slot::CUI_Item_Inven_Slot(const CUI_Item_Inven_Slot & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Item_Inven_Slot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Item_Inven_Slot::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	UIINFODESC* pDesc = (UIINFODESC*)pArg;
	if (nullptr == pDesc->pDesc)
		return E_FAIL;
	ITEMBUTTONDESC* pItemDesc = (ITEMBUTTONDESC*)pDesc->pDesc;
	lstrcpy(m_pIconTag, pItemDesc->pIconTag);


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fOriSize = m_UiInfo.fSizeX;
	m_fHoverSize = m_UiInfo.fSizeX + 20.f;



	Match_TextureWithModelName(); // 모델 이름으로 아이템을 비교하기 위해서 강제로 맞춘다.

	return S_OK;
}

void CUI_Item_Inven_Slot::Tick(_float fTimeDelta)
{

	if (UI_HOVER == m_eState)
	{
		if (m_fHoverSize > m_UiInfo.fSizeX)
		{
			m_UiInfo.fSizeX += 1.f;
			m_UiInfo.fSizeY += 1.f;
		}
		else
		{
			m_UiInfo.fSizeX = m_fHoverSize;
			m_UiInfo.fSizeY = m_fHoverSize;
		}
	}
	else
	{
		if (m_fHoverSize < m_UiInfo.fSizeX)
		{
			m_UiInfo.fSizeX -= 1.f;
			m_UiInfo.fSizeY -= 1.f;
		}
		else
		{
			m_UiInfo.fSizeX = m_fOriSize;
			m_UiInfo.fSizeY = m_fOriSize;
		}
	}



	__super::Tick(fTimeDelta);
}

void CUI_Item_Inven_Slot::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);
}

HRESULT CUI_Item_Inven_Slot::Render()
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


	// TODO

	//string str =  to_string(m_iCount);
	//TCHAR temp[MAX_PATH];
	//CToolManager::Get_Instance()->CtoTC(str.data(), temp);
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//pGameInstance->Render_Fonts(TEXT("Font_Nexon"), temp, _float2(XMVectorGetX(Get_TotalPos()), XMVectorGetY(Get_TotalPos())));
	//RELEASE_INSTANCE(CGameInstance);


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}




void CUI_Item_Inven_Slot::Match_TextureWithModelName()
{


	if (!lstrcmp(m_pIconTag, TEXT("Prototype_Component_Texture_Icon_SprintYarn")))
	{
		lstrcpy(m_pItemModelName, TEXT("yarn_ui_sprint"));
		lstrcpy(m_pHatModelName, TEXT("Sprint_Hat"));
	}
	else if (!lstrcmp(m_pIconTag, TEXT("Prototype_Component_Texture_Icon_WitchYarn")))
	{
		lstrcpy(m_pItemModelName, TEXT("yarn_ui_brew"));
		lstrcpy(m_pHatModelName, TEXT("Witch_Hat"));
	}
	else if (!lstrcmp(m_pIconTag, TEXT("Prototype_Component_Texture_Icon_IceYarn")))
	{
		lstrcpy(m_pItemModelName, TEXT("yarn_ui_ice"));
		lstrcpy(m_pHatModelName, TEXT("Ori_Hat"));
	}

}

HRESULT CUI_Item_Inven_Slot::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_pIconTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}





void CUI_Item_Inven_Slot::Handle_Hover()
{
	int a = 0;
}

void CUI_Item_Inven_Slot::Handle_Down()
{
	int a = 0;
}

void CUI_Item_Inven_Slot::Handle_Press()
{
	int a = 0;
}

void CUI_Item_Inven_Slot::Handle_Click()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);

	RELEASE_INSTANCE(CGameInstance);


	if (0 < m_iCount)
	{
		pPlayer->Get_Hat(m_pHatModelName);
	}


}



CUI_Item_Inven_Slot * CUI_Item_Inven_Slot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Item_Inven_Slot*		pInstance = new CUI_Item_Inven_Slot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Item_Inven_Slot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Item_Inven_Slot::Clone(void * pArg)
{
	CUI_Item_Inven_Slot*		pInstance = new CUI_Item_Inven_Slot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Item_Inven_Slot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUI_Item_Inven_Slot::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
