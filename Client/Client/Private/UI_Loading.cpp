#include "stdafx.h"
#include "..\Public\UI_Loading.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "UI_LoadingStart.h"


CUI_Loading::CUI_Loading(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_Loading::CUI_Loading(const CUI_Loading & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_Loading::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	

	Make_ChildUI(0.f, 0.f, g_iWinSizeX, g_iWinSizeY, TEXT("Prototype_UI_LoadingStart"));
	
	
	return S_OK;
}


void CUI_Loading::Set_Start(_bool bStart)
{
	if (m_bStart == bStart)
		return;

	m_bStart = bStart;
	((CUI_LoadingStart*)m_pChildUIs.front())->Set_Start(m_bStart);
}




void CUI_Loading::Tick(_float fTimeDelta)
{

	__super::Tick(fTimeDelta);
}

void CUI_Loading::LateTick(_float fTimeDelta)
{
	
	if(((CUI_LoadingStart*)m_pChildUIs.front())->Get_RealOff())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}



HRESULT CUI_Loading::Render()
{

	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));


	if (FAILED(m_pShaderCom->Begin(9)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CUI_Loading::Ready_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CUI_Loading * CUI_Loading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Loading*		pInstance = new CUI_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Loading::Clone(void * pArg)
{
	CUI_Loading*		pInstance = new CUI_Loading(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
}
