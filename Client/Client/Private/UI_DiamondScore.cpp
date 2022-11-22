#include "stdafx.h"
#include "..\Public\UI_DiamondScore.h"
#include "GameInstance.h"

#include "UI_Number.h"

CUI_DiamondScore::CUI_DiamondScore(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUI_DiamondScore::CUI_DiamondScore(const CUI_DiamondScore & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_DiamondScore::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_DiamondScore::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	Make_ChildUI(-40.f, 0.f, 50.f, 50.f, TEXT("Prototype_UI_Diamond"));
	Make_ChildUI(0.f, 0.f, 50.f, 50.f, TEXT("Prototype_UI_X"));
	Make_ChildUI(30.f, 0.f, 50.f, 50.f, TEXT("Prototype_UI_Number"));


	m_vOnPos.x = m_UiInfo.fX;
	m_vOnPos.y = m_UiInfo.fY;

	m_vOffPos.x = m_UiInfo.fX;
	m_vOffPos.y = m_UiInfo.fY + 100.f;

	m_bIsOn = false;
	m_UiInfo.fY = m_vOffPos.y;


	return S_OK;
}

void CUI_DiamondScore::Tick(_float fTimeDelta)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_M))
	{
		if (m_bIsOn)
			m_bIsOn = false;
		else
			m_bIsOn = true;
	}

	if (!m_bIsOn)
	{
		m_UiInfo.fY += 100.f * fTimeDelta;

		if (m_UiInfo.fY > m_vOffPos.y)
			m_UiInfo.fY = m_vOffPos.y;

	}
	else
	{
		m_UiInfo.fY -= 100.f * fTimeDelta;

		if (m_UiInfo.fY < m_vOnPos.y)
			m_UiInfo.fY = m_vOnPos.y;
	}

	RELEASE_INSTANCE(CGameInstance);


	__super::Tick(fTimeDelta);
}

void CUI_DiamondScore::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_DiamondScore::Render()
{
	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}

void CUI_DiamondScore::Set_Score(_uint iNum)
{
	list<CUI*>::iterator iter = m_pChildUIs.begin();
	++iter;
	++iter;
	((CUI_Number*)(*iter))->Set_Number(iNum);
}

HRESULT CUI_DiamondScore::Ready_Components()
{


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CUI_DiamondScore * CUI_DiamondScore::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_DiamondScore*		pInstance = new CUI_DiamondScore(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_DiamondScore"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_DiamondScore::Clone(void * pArg)
{
	CUI_DiamondScore*		pInstance = new CUI_DiamondScore(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_DiamondScore"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_DiamondScore::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
