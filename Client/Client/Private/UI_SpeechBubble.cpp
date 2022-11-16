#include "stdafx.h"
#include "..\Public\UI_SpeechBubble.h"
#include "GameInstance.h"

#include "ToolManager.h"



CUI_SpeechBubble::CUI_SpeechBubble(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_SpeechBubble::CUI_SpeechBubble(const CUI_SpeechBubble & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_SpeechBubble::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SpeechBubble::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eState = STATE_REAROFF;

	return S_OK;
}

void CUI_SpeechBubble::Tick(_float fTimeDelta)
{

	if (m_bShake)
	{
		m_fX = CToolManager::Get_Instance()->Get_RendomNum(-m_fPower, m_fPower);
		m_fY = CToolManager::Get_Instance()->Get_RendomNum(-m_fPower, m_fPower);
	}
	else
	{
		m_fX = 0.f;
		m_fY = 0.f;
	}

	switch (m_eState)
	{
	case STATE_ON:
		Tick_On(fTimeDelta);
		break;
	case STATE_IDLE:
		Tick_Idle(fTimeDelta);
		break;
	case STATE_OFF:
		Tick_Off(fTimeDelta);
		break;
	default:
		break;
	}



	// UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CUI_SpeechBubble::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);

	if(STATE_REAROFF != m_eState)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}


void CUI_SpeechBubble::Tick_On(_float fTimeDelta)
{
	m_fAlpa += fTimeDelta* 3.f;
	if (1.f < m_fAlpa)
	{
		m_fAlpa = 1.f;
		m_eState = STATE_IDLE;
		m_fTimeAcc = 0.f;
		m_iTextCount = 0;
	}
}

void CUI_SpeechBubble::Tick_Idle(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	m_fAlpa = 1.f;
	if (0.01f < m_fTimeAcc)
	{
		++m_iTextCount;
		if (MAX_PATH <= m_iTextCount)
			m_iTextCount = MAX_PATH - 1;

		m_fTimeAcc = 0.f;
	}
}

void CUI_SpeechBubble::Tick_Off(_float fTimeDelta)
{
	m_fAlpa -= fTimeDelta * 3.f;
	if (0.f > m_fAlpa)
	{
		m_fAlpa = 0.f;
		m_eState = STATE_REAROFF;
		m_fTimeAcc = 0.f;
		m_iTextCount = 0;
	}
}

void CUI_SpeechBubble::On_Text(TCHAR * sText, _float fSize, _float fPower, _bool bShake)
{
	if (!lstrcmp(m_sText, sText))
		return;

	m_eState = STATE_ON;
	lstrcpy(m_sText, sText);
	m_iTextCount = 0;
	m_fTimeAcc = 0.f;

	m_fAlpa = 0.f;
	m_bShake = bShake;
	m_fPower = fPower;
	m_fSize = fSize;
}

void CUI_SpeechBubble::Set_Text(TCHAR * sText, _float fSize, _float fPower, _bool bShake)
{
	if (!lstrcmp(m_sText, sText))
		return;

	m_eState = STATE_IDLE;
	lstrcpy(m_sText, sText);
	m_iTextCount = 0;
	m_fTimeAcc = 0.f;

	m_fAlpa = 1.f;
	m_bShake = bShake;
	m_fPower = fPower;
	m_fSize = fSize;
}

void CUI_SpeechBubble::Off_Text()
{
	m_eState = STATE_OFF;

	m_fAlpa = 1.f;
}




HRESULT CUI_SpeechBubble::Render()
{
	// 자식들의 Render 호출
	__super::Render();

	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	m_pShaderCom->Set_RawValue("g_Alpa", &m_fAlpa, sizeof(_float));

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (STATE_IDLE == m_eState)
	{
		TCHAR sTempText[MAX_PATH];
		ZeroMemory(sTempText, sizeof(TCHAR) * MAX_PATH);
		for (_uint i = 0; i < m_iTextCount; ++i)
		{
			sTempText[i] = m_sText[i];
		}

		CToolManager::Get_Instance()->Render_FontsY(TEXT("Font_Nexon"), sTempText, _float2(m_UiInfo.fX * 0.5f + 20.f + m_fX, m_UiInfo.fY + m_fY), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), _float2(m_fSize, m_fSize));
	}



	// UI_RenderDebug();

	return S_OK;
}





HRESULT CUI_SpeechBubble::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_speech_bubble"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CUI_SpeechBubble * CUI_SpeechBubble::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_SpeechBubble*		pInstance = new CUI_SpeechBubble(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_SpeechBubble::Clone(void * pArg)
{
	CUI_SpeechBubble*		pInstance = new CUI_SpeechBubble(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_SpeechBubble::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
