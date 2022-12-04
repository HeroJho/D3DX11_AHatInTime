#include "stdafx.h"
#include "..\Public\UI_LoadingStart.h"
#include "GameInstance.h"

#include "ToolManager.h"


CUI_LoadingStart::CUI_LoadingStart(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_LoadingStart::CUI_LoadingStart(const CUI_LoadingStart & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_LoadingStart::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LoadingStart::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}


void CUI_LoadingStart::Set_Start(_bool bStart)
{

	LEVEL eLEVEL = CToolManager::Get_Instance()->Get_CulLevel();

	if (LEVEL_BOSS == eLEVEL && LEVEL_ENDING == eLEVEL)
	{
		m_bStart = bStart;
		m_bOnOff = true;
		return;
	}

	if (bStart)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->PlaySoundW(L"Stage Enter Woosh 1.mp3", SOUND_BGM1, g_fEffectSound + 1.f);
		RELEASE_INSTANCE(CGameInstance);
	}
	else
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->StopSound(SOUND_BGM);
		pGameInstance->PlaySoundW(L"Stage Enter Woosh 2.mp3", SOUND_BGM1, g_fEffectSound + 1.f);
		RELEASE_INSTANCE(CGameInstance);
	}

	m_bStart = bStart;

	m_bOnOff = true;

}




void CUI_LoadingStart::Tick(_float fTimeDelta)
{
	if (0.f == m_fScale && !m_bStart)
	{
		m_fAngle = 0.f;
		m_fScale = 0.f;
		m_bRealOff = false;
		return;
	}
	m_bRealOff = true;

	if (m_bStart)
	{
		m_fAngle += fTimeDelta * 100.f;
		m_fScale += fTimeDelta * 10.f;

		if (50.f < m_fScale)
			m_fScale = 50.f;
	}
	else
	{
		m_fAngle += fTimeDelta * 100.f;
		m_fScale -= fTimeDelta * 10.f;

		if (0.f > m_fScale)
			m_fScale = 0.f;
	}

	__super::Tick(fTimeDelta);
}

void CUI_LoadingStart::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}



HRESULT CUI_LoadingStart::Render()
{
	if (!m_bOnOff && !m_bStart)
		return S_OK;

	// 자식들의 Render 호출
	__super::Render();

	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));


	
	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 2)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Bind_SRV(TEXT("Target_Blend"), m_pShaderCom, "g_BlendTexture")))
		return E_FAIL;




	_matrix mAngle = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_fAngle));
	_matrix mScale = XMMatrixScaling((1280.f / 720.f) * 0.1f + m_fScale, 1.f * 0.1f + m_fScale, 1.f);
	_matrix mTotal = mScale * mAngle;
	_float4x4 mTotals; XMStoreFloat4x4(&mTotals, XMMatrixTranspose(mTotal));

	m_pShaderCom->Set_RawValue("g_StartMatrix", &mTotals, sizeof(_float4x4));







	RELEASE_INSTANCE(CGameInstance);


	if (FAILED(m_pShaderCom->Begin(10)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}






HRESULT CUI_LoadingStart::Ready_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	

	return S_OK;
}

CUI_LoadingStart * CUI_LoadingStart::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_LoadingStart*		pInstance = new CUI_LoadingStart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_LoadingStart::Clone(void * pArg)
{
	CUI_LoadingStart*		pInstance = new CUI_LoadingStart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LoadingStart::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

}
