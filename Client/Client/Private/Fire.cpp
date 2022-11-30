#include "stdafx.h"
#include "..\Public\Fire.h"
#include "GameInstance.h"

#include "MapManager.h"
#include "ToolManager.h"
#include "GameManager.h"

#include "Light.h"


CFire::CFire(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CFire::CFire(const CFire & rhs)
	: CGameObject(rhs)
{

}




HRESULT CFire::Initialize_Prototype()
{



	return S_OK;
}

HRESULT CFire::Initialize(void * pArg)
{

	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_Desc = *(FIREDESC*)pArg;

	m_sTag = "Tag_Fire";


	_vector vPos = XMLoadFloat3(&m_Desc.vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_Desc.vScale));


	// 빛 정의
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vDiffuse = m_Desc.vDiffuseColor;
	LightDesc.vAmbient = m_Desc.vAmColor;

	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 0.5f);
	XMStoreFloat4(&LightDesc.vPosition, XMVectorSetW(vPos, 1.f));

	LightDesc.fRange = m_Desc.fPower;

	pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, &m_pLight);

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}








void CFire::Tick(_float fTimeDelta)
{

	// 켜서 가만히 있는다

	if (0.8f < m_fRatio)
	{
		m_bTurn = true;
	}
	else if (0.7f > m_fRatio)
	{
		m_bTurn = false;
	}

	if (m_bTurn)
	{
		m_fRatio -= m_Desc.fDownSpeed * fTimeDelta;
	}
	else
	{
		m_fRatio += m_Desc.fUpSpeed * fTimeDelta;
	}

	if (m_pLight)
	{
		_float4 vDiffuColor(m_Desc.vDiffuseColor.x *m_fRatio, m_Desc.vDiffuseColor.y *m_fRatio, m_Desc.vDiffuseColor.z *m_fRatio, 1.f);
		_float4 vAmbColor(m_Desc.vAmColor.x *m_fRatio, m_Desc.vAmColor.y *m_fRatio, m_Desc.vAmColor.z *m_fRatio, 1.f);
		m_pLight->Get_LightEditDesc()->vDiffuse = vDiffuColor;
		m_pLight->Get_LightEditDesc()->vAmbient = vAmbColor;
	}

}






void CFire::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_fDeltaTime += fTimeDelta;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 빌보드
	_matrix mCam = pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);
	_vector vCamPos = mCam.r[3];
	m_pTransformCom->LookAtInv(vCamPos, 0.f);


	 

	_bool bRender = !(CGameManager::Get_Instance()->Check_IsInWisp(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) || pGameInstance->Get_Dark());

	RELEASE_INSTANCE(CGameInstance);

	// 소팅
	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if(bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}







HRESULT CFire::Render()
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



	if (FAILED(m_pShaderCom->Set_RawValue("g_DeltaTime", &m_fDeltaTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom0->Set_SRV(m_pShaderCom, "g_Noise", 0)))
		return E_FAIL;

	_float3 vScrollSpeeds(1.3f, 2.1f, 2.3f);
	_float3 vScales(1.f, 2.f, 3.f);
	_float2 vDistortion1(0.1f, 0.2f);
	_float2 vDistortion2(0.1f, 0.3f);
	_float2 vDistortion3(0.1f, 0.1f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_ScrollSpeeds", &vScrollSpeeds, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Scales", &vScales, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Distortion1", &vDistortion1, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Distortion2", &vDistortion2, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Distortion3", &vDistortion3, sizeof(_float2))))
		return E_FAIL;



	if (FAILED(m_pShaderCom->Set_RawValue("g_DistortionScale", &m_fDistortionScale, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_DistortionBias", &m_fDistortionBias, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom0->Set_SRV(m_pShaderCom, "g_Mask", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom1->Set_SRV(m_pShaderCom, "g_Noise", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	return S_OK;
}







HRESULT CFire::Ready_Components()
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


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Fire_D"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return S_OK;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Fire_M"), TEXT("Com_Texture0"), (CComponent**)&m_pTextureCom0)))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Fire_Noise"), TEXT("Com_Texture1"), (CComponent**)&m_pTextureCom1)))
		return E_FAIL;


	return S_OK;
}






CFire * CFire::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFire*		pInstance = new CFire(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFire"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFire::Clone(void * pArg)
{
	CFire*		pInstance = new CFire(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFire"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFire::Free()
{
	__super::Free();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom0);
	Safe_Release(m_pTextureCom1);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
