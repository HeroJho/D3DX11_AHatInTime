#include "stdafx.h"
#include "..\Public\Wind_Spiral.h"
#include "GameInstance.h"

#include "MapManager.h"
#include "MeshManager.h"
#include "ToolManager.h"


CWind_Spiral::CWind_Spiral(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CWind_Spiral::CWind_Spiral(const CWind_Spiral & rhs)
	: CGameObject(rhs)
{

}




HRESULT CWind_Spiral::Initialize_Prototype()
{



	return S_OK;
}

HRESULT CWind_Spiral::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(180.f));
	m_pTransformCom->Set_Scale(XMVectorSet(1.f, 0.5f, 1.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));


	m_vOriScale = _float3(1.f, 1.f, 1.f);
	m_fMaxScale = m_vOriScale.x * 5.f;

	return S_OK;
}








void CWind_Spiral::Tick(_float fTimeDelta)
{

	_float3 vScale = m_pTransformCom->Get_Scale();
	_float fTempScale = vScale.x;
	vScale.x += m_fScaleSpeed * fTimeDelta;

	fTempScale = fTempScale / vScale.x;

	m_pTransformCom->Set_Scale(XMVectorSet(vScale.x, vScale.y * fTempScale, vScale.x, 1.f));

	m_fRatio = vScale.x / m_fMaxScale;
	if (1.f < m_fRatio)
		m_b = true;

}









void CWind_Spiral::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_fDeltaTime += fTimeDelta;

	if (!m_b)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}







HRESULT CWind_Spiral::Render()
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

	if (FAILED(m_pTextureCom0->Set_SRV(m_pShaderCom, "g_Noise", 0)))
		return E_FAIL;

	_float3 vScrollSpeeds(1.f, 2.f, 3.f);
	_float3 vScales(2.f, 3.f, 4.f);
	_float2 vDistortion1(1.f, 1.f);
	_float2 vDistortion2(1.f, 1.f);
	_float2 vDistortion3(1.f, 1.f);
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

	_float fAlpa = 1.f - m_fRatio;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpa", &fAlpa, sizeof(_float))))
		return E_FAIL;
	

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 6)))
			return E_FAIL;
	}



	return S_OK;
}







HRESULT CWind_Spiral::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("wind_spiral"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_distortionclouds_tex"), TEXT("Com_Texture0"), (CComponent**)&m_pTextureCom0)))
		return E_FAIL;
	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ColorNoise"), TEXT("Com_Texture1"), (CComponent**)&m_pTextureCom1)))
	//	return E_FAIL;

	return S_OK;
}






CWind_Spiral * CWind_Spiral::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWind_Spiral*		pInstance = new CWind_Spiral(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWind_Spiral"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWind_Spiral::Clone(void * pArg)
{
	CWind_Spiral*		pInstance = new CWind_Spiral(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CWind_Spiral"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWind_Spiral::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom0);
	Safe_Release(m_pTextureCom1);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}

void CWind_Spiral::Tick_ImGui(_float fTimeDelta)
{
	ImGui::Begin("CWind_Spiral");


	if (ImGui::Button("Replay"))
	{
		m_b = false;
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vOriScale));
	}



	ImGui::InputFloat("ScaleSpeed", &m_fScaleSpeed);


	ImGui::InputFloat3("m_vOriScale ", (float*)&m_vOriScale);
	ImGui::InputFloat("m_fMaxScale", &m_fMaxScale);
	
	_float3 vScale = m_pTransformCom->Get_Scale();
	if (ImGui::DragFloat3("Scale", (float*)&vScale))
		m_pTransformCom->Set_Scale(XMVectorSetW(XMLoadFloat3(&vScale), 1.f));




	_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (ImGui::DragFloat3("vPos", (float*)&vPos))
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));

	if (ImGui::DragFloat("Angle", &m_fAnlge))
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fAnlge);


	ImGui::End();
}
