#include "stdafx.h"
#include "..\Public\UI_ItemButton.h"
#include "GameInstance.h"

CUI_ItemButton::CUI_ItemButton(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUI_ItemButton::CUI_ItemButton(const CUI_ItemButton & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_ItemButton::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ItemButton::Initialize(void * pArg)
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
	
	_vector vSentorPos = { 0.f, 0.f };
	_vector vMyPos = { m_UiInfo.fX, m_UiInfo.fY };
	_vector vDis = XMVector2Normalize(vMyPos - vSentorPos);

	XMStoreFloat2(&m_vDir, vDis);

	XMStoreFloat2(&m_vStartPos, vMyPos + vDis * 30.f);
	XMStoreFloat2(&m_vEndPos, vMyPos);

	return S_OK;
}

void CUI_ItemButton::Tick(_float fTimeDelta)
{
	
	switch (m_eItemUIState)
	{
	case Client::CUI_ItemButton::ITEMUI_START:
		Tick_Start(fTimeDelta);
		break;
	case Client::CUI_ItemButton::ITEMUI_IDEL:
		Tick_Idel(fTimeDelta);
		break;
	case Client::CUI_ItemButton::ITEMUI_EEND:
		Tick_End(fTimeDelta);
		break;
	}



	__super::Tick(fTimeDelta);
}

void CUI_ItemButton::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);
}

HRESULT CUI_ItemButton::Render()
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



	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}

void CUI_ItemButton::Start()
{
	m_eItemUIState = ITEMUI_START;
}

void CUI_ItemButton::End()
{
	m_eItemUIState = ITEMUI_EEND;
}



void CUI_ItemButton::Tick_Start(_float fTimeDelta)
{
	// m_fChangeTimeAcc += fTimeDelta;
	
	_vector vPos = { m_UiInfo.fX, m_UiInfo.fY, 0.f, 0.f };

	_float fDis = XMVectorGetX(XMVector2Length(vPos - XMLoadFloat2(&m_vEndPos)));

	if (2.f > fDis || 50.f < fDis)
	{
		m_UiInfo.fX = m_vEndPos.x;
		m_UiInfo.fY = m_vEndPos.y;
		m_eItemUIState = ITEMUI_IDEL;
	}
	else
	{
		vPos += -1.f * XMLoadFloat2(&m_vDir) * 100.f * fTimeDelta;
		m_UiInfo.fX = XMVectorGetX(vPos);
		m_UiInfo.fY = XMVectorGetY(vPos);
	}

}

void CUI_ItemButton::Tick_Idel(_float fTimeDelta)
{

}

void CUI_ItemButton::Tick_End(_float fTimeDelta)
{
	// m_fChangeTimeAcc += fTimeDelta;

	_vector vPos = { m_UiInfo.fX, m_UiInfo.fY, 0.f, 0.f };

	_float fDis = XMVectorGetX(XMVector2Length(vPos - XMLoadFloat2(&m_vStartPos)));

	if (2.f > fDis || 50.f < fDis)
	{
		// TODO: Iven 닫기

		m_eItemUIState = ITEMUI_END;
		m_UiInfo.fX = m_vStartPos.x;
		m_UiInfo.fY = m_vStartPos.y;
	}
	else
	{
		vPos += XMLoadFloat2(&m_vDir) * 100.f * fTimeDelta;
		m_UiInfo.fX = XMVectorGetX(vPos);
		m_UiInfo.fY = XMVectorGetY(vPos);
	}
}





HRESULT CUI_ItemButton::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_pIconTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}





void CUI_ItemButton::Handle_Hover()
{
	int a = 0;
}

void CUI_ItemButton::Handle_Down()
{
	int a = 0;
}

void CUI_ItemButton::Handle_Press()
{
	int a = 0;
}

void CUI_ItemButton::Handle_Click()
{
	int a = 0;
}



CUI_ItemButton * CUI_ItemButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_ItemButton*		pInstance = new CUI_ItemButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_ItemButton"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_ItemButton::Clone(void * pArg)
{
	CUI_ItemButton*		pInstance = new CUI_ItemButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_ItemButton"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUI_ItemButton::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
