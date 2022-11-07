#include "stdafx.h"
#include "..\Public\UI_Inven.h"
#include "GameInstance.h"

#include "ItemManager.h"
#include "ToolManager.h"

#include "UI_ItemButton.h"


CUI_Inven::CUI_Inven(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_Inven::CUI_Inven(const CUI_Inven & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_Inven::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Inven::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;




	return S_OK;
}

void CUI_Inven::Tick(_float fTimeDelta)
{
	if (m_pChildUIs.empty())
		return;

	if (CUI_ItemButton::ITEMUI_END == ((CUI_ItemButton*)m_pChildUIs.front())->Get_State())
	{
		m_UiInfo.fX = FLT_MAX;
		m_UiInfo.fY = FLT_MAX;
	}


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(DIK_TAB))
	{
		m_bSwitch = true;

		m_UiInfo.fX = g_iWinSizeX * 0.5f;
		m_UiInfo.fY = g_iWinSizeY * 0.5f;
		Start();
		CToolManager::Get_Instance()->Set_All(0.1f);
	}
	else
	{
		if (m_bSwitch)
		{
			CToolManager::Get_Instance()->Set_All(1.f);
			m_bSwitch = false;
		}



		End();
	}

	RELEASE_INSTANCE(CGameInstance);



	__super::Tick(fTimeDelta);
}

void CUI_Inven::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Inven::Render()
{
	// 자식들의 Render 호출
	__super::Render();


	return S_OK;
}

void CUI_Inven::LoadItemMgr_ItemUI()
{
	Delete_AllChildUI();

	list<CItemManager::HATINFODESC>* items = CItemManager::Get_Instance()->Get_Hats();

	_float fDis = 150.f;

	_float iAngle = 0.f;
	 
	_vector vInitDir = { 0.f, -1.f, 0.f, 0.f };

	for (auto& Desc : *items)
	{
		iAngle += 360.f / items->size();

		_float2 vPos;
		_matrix mRot = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(iAngle));

		XMStoreFloat2(&vPos, XMVector2Normalize(XMVector2TransformNormal(vInitDir, mRot)) * fDis);




		CUI_ItemButton::ITEMBUTTONDESC IconDesc;
		if (!lstrcmp(Desc.szModelName, TEXT("Ori_Hat")))
		{
			lstrcpy(IconDesc.pIconTag, TEXT("Prototype_Component_Texture_Icon_KidHat"));
			Make_ChildUI(vPos.x, vPos.y, 256.f / 3.f, 256.f / 3.f, TEXT("Prototype_UI_ItemButton"), &IconDesc);
		}
		else if (!lstrcmp(Desc.szModelName, TEXT("Sprint_Hat")))
		{
			lstrcpy(IconDesc.pIconTag, TEXT("Prototype_Component_Texture_Icon_SprintHat"));
			Make_ChildUI(vPos.x, vPos.y, 256.f / 3.f, 256.f / 3.f, TEXT("Prototype_UI_ItemButton"), &IconDesc);
		}
		else if (!lstrcmp(Desc.szModelName, TEXT("Witch_Hat")))
		{
			lstrcpy(IconDesc.pIconTag, TEXT("Prototype_Component_Texture_Icon_WitchHat"));
			Make_ChildUI(vPos.x, vPos.y, 256.f / 3.f, 256.f / 3.f, TEXT("Prototype_UI_ItemButton"), &IconDesc);
		}
		else if (!lstrcmp(Desc.szModelName, TEXT("Mask_Cat")))
		{
			lstrcpy(IconDesc.pIconTag, TEXT("Prototype_Component_Texture_DwellerMask"));
			Make_ChildUI(vPos.x, vPos.y, 256.f / 3.f, 256.f / 3.f, TEXT("Prototype_UI_ItemButton"), &IconDesc);
		}
		else if (!lstrcmp(Desc.szModelName, TEXT("Mask_Fox")))
		{
			lstrcpy(IconDesc.pIconTag, TEXT("Prototype_Component_Texture_Foxmask"));
			Make_ChildUI(vPos.x, vPos.y, 256.f / 3.f, 256.f / 3.f, TEXT("Prototype_UI_ItemButton"), &IconDesc);
		}


	}

}

void CUI_Inven::Start()
{

	for (auto& pChild : m_pChildUIs)
	{
		((CUI_ItemButton*)pChild)->Start();
	}

}

void CUI_Inven::End()
{

	for (auto& pChild : m_pChildUIs)
	{
		((CUI_ItemButton*)pChild)->End();
	}

}

HRESULT CUI_Inven::Ready_Components()
{

	///* For.Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenFrame"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	//	return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CUI_Inven * CUI_Inven::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Inven*		pInstance = new CUI_Inven(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Inven::Clone(void * pArg)
{
	CUI_Inven*		pInstance = new CUI_Inven(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Inven::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
