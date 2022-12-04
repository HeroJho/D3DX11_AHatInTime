#include "stdafx.h"
#include "..\Public\Splash_wave.h"
#include "GameInstance.h"


#include "ToolManager.h"
#include "Player.h"


CSplash_wave::CSplash_wave(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CSplash_wave::CSplash_wave(const CSplash_wave & rhs)
	: CGameObject(rhs)
{

}




HRESULT CSplash_wave::Initialize_Prototype()
{



	return S_OK;
}

HRESULT CSplash_wave::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	SPLASHWAVEDESC* pDesc = (SPLASHWAVEDESC*)pArg;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));

	m_pTransformCom->Set_Scale(XMVectorSet(1.f, 2.f, 1.f, 1.f));

	return S_OK;
}








void CSplash_wave::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->PlaySoundW(L"Oil_loop1.wav", SOUND_PEFFECT_SPRT, g_fEffectSound - 0.2f, true);


	m_fDeltaTime += fTimeDelta;

	_float3 vScale = m_pTransformCom->Get_Scale();
	vScale.x += m_fScaleSpeed * fTimeDelta;
	vScale.z += m_fScaleSpeed * fTimeDelta;
	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));

	if (40.f < vScale.x)
		Set_Dead(true);


	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr(LEVEL_STATIC, TEXT("Layer_Player"), 0);
	RELEASE_INSTANCE(CGameInstance);

	CTransform* pTran = (CTransform*)pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
	_vector vPos = pTran->Get_State(CTransform::STATE_POSITION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDis = XMVectorGetX(XMVector3Length(vPos - vMyPos));

	vScale.x *= 0.5f;

	if (vScale.x - 0.1f < fDis && vScale.x + 0.2f > fDis)
	{
		if (CPlayer::STATE_JUMPATTACK != pPlayer->Get_State() &&
			false == pPlayer->Get_Attacked() &&
			pPlayer->Get_IsPlayerOn())
		{
			pPlayer->Attacked();
		}
	}



}









void CSplash_wave::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;



	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}







HRESULT CSplash_wave::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);



	if (FAILED(m_pShaderCom->Set_RawValue("g_DeltaTime", &m_fDeltaTime, sizeof(_float))))
		return E_FAIL;

	_bool bBlur = true;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bBlur", &bBlur, sizeof(_bool))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 12)))
			return E_FAIL;
	}



	return S_OK;
}







HRESULT CSplash_wave::Ready_Components()
{

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return S_OK;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("splash_wave"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;




	return S_OK;
}






CSplash_wave * CSplash_wave::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSplash_wave*		pInstance = new CSplash_wave(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSplash_wave"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSplash_wave::Clone(void * pArg)
{
	CSplash_wave*		pInstance = new CSplash_wave(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSplash_wave"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplash_wave::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}

