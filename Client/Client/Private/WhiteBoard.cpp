#include "stdafx.h"
#include "..\Public\WhiteBoard.h"
#include "GameInstance.h"

#include "ToolManager.h"



CWhiteBoard::CWhiteBoard(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CWhiteBoard::CWhiteBoard(const CWhiteBoard & rhs)
	: CUI(rhs)
{

}

HRESULT CWhiteBoard::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWhiteBoard::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CWhiteBoard::Tick(_float fTimeDelta)
{

	if (!m_bStart)
		return;

	m_fAlpa += fTimeDelta * m_fAlpaSpeed;
	if (1.f < m_fAlpa)
		m_fAlpa = 1.f;

	__super::Tick(fTimeDelta);
}

void CWhiteBoard::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	if(m_bStart)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}



HRESULT CWhiteBoard::Render()
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


	if (FAILED(m_pShaderCom->Begin(6)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	return S_OK;
}

void CWhiteBoard::Set_Start(_bool bStart)
{
	if (m_bStart == bStart)
		return;

	m_bStart = bStart;

	if (m_bStart)
	{
		m_fAlpa = 0.f;
	}
	else
	{
		m_fAlpa = 0.f;
	}

}





HRESULT CWhiteBoard::Ready_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CWhiteBoard * CWhiteBoard::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWhiteBoard*		pInstance = new CWhiteBoard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWhiteBoard::Clone(void * pArg)
{
	CWhiteBoard*		pInstance = new CWhiteBoard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWhiteBoard::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
}
