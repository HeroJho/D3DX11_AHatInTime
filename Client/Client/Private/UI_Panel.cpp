#include "stdafx.h"
#include "..\Public\UI_Panel.h"
#include "GameInstance.h"

CUI_Panel::CUI_Panel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Panel::CUI_Panel(const CUI_Panel & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Panel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Panel::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_Panel::Tick(_float fTimeDelta)
{




	__super::Tick(fTimeDelta);
}

void CUI_Panel::LateTick(_float fTimeDelta)
{





	__super::LateTick(fTimeDelta);
}

HRESULT CUI_Panel::Render()
{


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}

HRESULT CUI_Panel::Ready_Components()
{



	return S_OK;
}

CUI_Panel * CUI_Panel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Panel*		pInstance = new CUI_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Panel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Panel::Clone(void * pArg)
{
	CUI_Panel*		pInstance = new CUI_Panel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Panel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Panel::Free()
{
	__super::Free();
}
