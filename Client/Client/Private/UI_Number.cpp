#include "stdafx.h"
#include "..\Public\UI_Number.h"
#include "GameInstance.h"



CUI_Number::CUI_Number(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_Number::CUI_Number(const CUI_Number & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_Number::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Number::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_fMaxScale = m_UiInfo.fSizeX + 100.f;
	m_fNomalScale = m_UiInfo.fSizeX;

	Set_Number(0);

	return S_OK;
}

void CUI_Number::Tick(_float fTimeDelta)
{
	if (m_bScale)
	{
		if (m_fNomalScale > m_UiInfo.fSizeX)
		{
			m_bScale = false;
			m_UiInfo.fSizeX = m_fNomalScale;
			m_UiInfo.fSizeY = m_fNomalScale;
		}
		else
		{
			m_UiInfo.fSizeX -= fTimeDelta * 500.f;
			m_UiInfo.fSizeY -= fTimeDelta * 500.f;
		}
	}
		
	
	__super::Tick(fTimeDelta);
}

void CUI_Number::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);
}

HRESULT CUI_Number::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	

	_float4x4 mStoreMax;
	
	_float fX = m_TotalInfo.fX;

	for (_uint i = 0; i < m_sNumber.size(); ++i)
	{
		_uint iIndex = m_sNumber[i] - 48;

		_matrix mMax = m_pTransformCom->Get_WorldMatrix();
		mMax.r[3] = XMVectorSet(fX - g_iWinSizeX * 0.5f, -m_TotalInfo.fY + g_iWinSizeY * 0.5f, 0.0f, 1.f);


		mMax = XMMatrixTranspose(mMax);
		XMStoreFloat4x4(&mStoreMax, mMax);
		m_pShaderCom->Set_RawValue("g_WorldMatrix", &mStoreMax, sizeof(_float4x4));

		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", iIndex)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;


 		fX += m_UiInfo.fSizeX * 0.5f;
	}


	// 자식들의 Render 호출
	__super::Render();


	return S_OK;
}

void CUI_Number::Set_Number(_uint iNum)
{
	m_iNumber = iNum;
	m_sNumber = to_string(m_iNumber);
	Start_Scale();
}

void CUI_Number::Start_Scale()
{
	m_bScale = true;
	m_UiInfo.fSizeX = m_fMaxScale;
	m_UiInfo.fSizeY = m_fMaxScale;
}


HRESULT CUI_Number::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Number"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CUI_Number * CUI_Number::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Number*		pInstance = new CUI_Number(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Number::Clone(void * pArg)
{
	CUI_Number*		pInstance = new CUI_Number(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Number::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
