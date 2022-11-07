#include "stdafx.h"
#include "..\Public\UI_Item_Inven.h"
#include "GameInstance.h"

#include "ItemManager.h"

#include "UI_Item_Inven_Slot.h"



CUI_Item_Inven::CUI_Item_Inven(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_Item_Inven::CUI_Item_Inven(const CUI_Item_Inven & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_Item_Inven::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Item_Inven::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;




	CUI_Item_Inven_Slot::ITEMBUTTONDESC IconDesc;
	lstrcpy(IconDesc.pIconTag, TEXT("Prototype_Component_Texture_Icon_SprintYarn"));
	Make_ChildUI(100.f, -50.f, 256.f / 4.f, 256.f / 4.f, TEXT("Prototype_UI_Item_Inven_Slot"), &IconDesc);

	lstrcpy(IconDesc.pIconTag, TEXT("Prototype_Component_Texture_Icon_WitchYarn"));
	Make_ChildUI(0.f, -50.f, 256.f / 4.f, 256.f / 4.f, TEXT("Prototype_UI_Item_Inven_Slot"), &IconDesc);

	lstrcpy(IconDesc.pIconTag, TEXT("Prototype_Component_Texture_Icon_IceYarn"));
	Make_ChildUI(-100.f, -50.f, 256.f / 4.f, 256.f / 4.f, TEXT("Prototype_UI_Item_Inven_Slot"), &IconDesc);


	return S_OK;
}

void CUI_Item_Inven::Tick(_float fTimeDelta)
{


	__super::Tick(fTimeDelta);
}

void CUI_Item_Inven::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Item_Inven::Render()
{
	// 자식들의 Render 호출
	__super::Render();


	return S_OK;
}

void CUI_Item_Inven::LoadItemMgr_ItemUI()
{

	list<CItemManager::ITEMINFODESC>* items = CItemManager::Get_Instance()->Get_Items();


	for (auto& pChild : m_pChildUIs)
	{
		CUI_Item_Inven_Slot* pSlot = (CUI_Item_Inven_Slot*)pChild;

		for (auto& pItem : *items)
		{
			if (!lstrcmp(pSlot->Get_ItemModelTag(), pItem.szModelName))
			{
				pSlot->Set_Count(pItem.iCount);
				break;
			}
		}
	}

}


HRESULT CUI_Item_Inven::Ready_Components()
{

	///* For.Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenFrame"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	//	return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CUI_Item_Inven * CUI_Item_Inven::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Item_Inven*		pInstance = new CUI_Item_Inven(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Item_Inven::Clone(void * pArg)
{
	CUI_Item_Inven*		pInstance = new CUI_Item_Inven(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Item_Inven::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
