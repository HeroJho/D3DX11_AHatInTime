#include "stdafx.h"
#include "..\Public\CheckPoint.h"
#include "GameInstance.h"

#include "ToolManager.h"



CCheckPoint::CCheckPoint(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CCheckPoint::CCheckPoint(const CCheckPoint & rhs)
	: CUI(rhs)
{

}

HRESULT CCheckPoint::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCheckPoint::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_fOnX = m_UiInfo.fX;
	m_fOffX = m_UiInfo.fX + 350.f;

	return S_OK;
}

void CCheckPoint::Tick(_float fTimeDelta)
{

	if (m_bOnOff)
	{
		m_fTimeAcc += fTimeDelta;

		if (10.f < m_fTimeAcc)
		{
			m_bOnOff = false;
			m_fTimeAcc = 0.f;
		}
	}
	else
	{
		m_fTimeAcc = 0.f;
	}





	if (m_bOnOff)
	{
		m_UiInfo.fX -= fTimeDelta * 400.f;

		if (m_fOnX > m_UiInfo.fX)
			m_UiInfo.fX = m_fOnX;
	}
	else
	{
		m_UiInfo.fX += fTimeDelta * 400.f;

		if (m_fOffX < m_UiInfo.fX)
			m_UiInfo.fX = m_fOffX;
	}


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CCheckPoint::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}



HRESULT CCheckPoint::Render()
{
	// 자식들의 Render 호출
	__super::Render();

	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	_bool bBlur = false;
	m_pShaderCom->Set_RawValue("g_bBlur", &bBlur, sizeof(_bool));

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;





	//UI_RenderDebug();

	return S_OK;
}





HRESULT CCheckPoint::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Checkpoint"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CCheckPoint * CCheckPoint::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCheckPoint*		pInstance = new CCheckPoint(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCheckPoint::Clone(void * pArg)
{
	CCheckPoint*		pInstance = new CCheckPoint(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCheckPoint::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
