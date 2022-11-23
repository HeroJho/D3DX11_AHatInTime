#include "stdafx.h"
#include "..\Public\WitchChargEffect.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "GameManager.h"
#include "ParticleManager.h"

#include "Light.h"
#include "Player.h"

CWitchChargEffect::CWitchChargEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CWitchChargEffect::CWitchChargEffect(const CWitchChargEffect & rhs)
	: CGameObject(rhs)
{
}






HRESULT CWitchChargEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWitchChargEffect::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_sTag = "Tag_Effect";


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(2.f, 2.f, 2.f, 1.f));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr(LEVEL_STATIC, TEXT("Layer_Player"), 0);
	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}



void CWitchChargEffect::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);


	CTransform* pTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
	_vector vPPos = pTran->Get_State(CTransform::STATE_POSITION);
	vPPos = XMVectorSetY(vPPos, XMVectorGetY(vPPos) + 0.5f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPPos);



	m_fAngle += 500.f * fTimeDelta;

	m_fRatioAcc -= 0.9f * fTimeDelta;
	m_fRatio = m_fRatioAcc / 1.f;
	_float3 fScale = m_pTransformCom->Get_Scale();
	fScale.x *= m_fRatioAcc;
	m_pTransformCom->Set_Scale(XMVectorSet(fScale.x, fScale.x, fScale.x, 1.f));


	m_fTimeAcc += fTimeDelta;
	if (1.f < m_fTimeAcc)
	{
		_float3 vXPos; XMStoreFloat3(&vXPos, vPPos);
		_float3 vLook; XMStoreFloat3(&vLook, pTran->Get_State(CTransform::STATE_LOOK));
		
		CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_Star"), vXPos, _float3(0.f, 0.15f, 0.f), vLook, _float3(0.5f, 0.5f, 0.5f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.1f, 3.f, true, 1.f, 1.f, 1.f,
			5, 0.f, 0.2f, 0.f, 0.f, 0.f, 0.2f, 0.f, 0.1f, 1.f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_TEXTURE);
		Set_Dead(true);
	}

}




void CWitchChargEffect::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// ºôº¸µå
	_matrix mCam = pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);
	_vector vCamPos = mCam.r[3];


	m_pTransformCom->LookAtInv(vCamPos, m_fAngle);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance);

}



HRESULT CWitchChargEffect::Render()
{
	if (nullptr == m_pVIBufferCom ||
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


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

void CWitchChargEffect::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{



}




















HRESULT CWitchChargEffect::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_star_shuriken"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}










CWitchChargEffect * CWitchChargEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWitchChargEffect*		pInstance = new CWitchChargEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWitchChargEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWitchChargEffect::Clone(void * pArg)
{
	CWitchChargEffect*		pInstance = new CWitchChargEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CWitchChargEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWitchChargEffect::Free()
{
	__super::Free();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
