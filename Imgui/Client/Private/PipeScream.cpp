#include "stdafx.h"
#include "..\Public\PipeScream.h"
#include "GameInstance.h"

#include "MapManager.h"
#include "MeshManager.h"
#include "ToolManager.h"


CPipeScream::CPipeScream(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CPipeScream::CPipeScream(const CPipeScream & rhs)
	: CGameObject(rhs)
{

}




HRESULT CPipeScream::Initialize_Prototype()
{



	return S_OK;
}

HRESULT CPipeScream::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_pTransformCom->Set_Scale(XMVectorSet(0.05f, 0.1f, 0.05f, 1.f));

	return S_OK;
}








void CPipeScream::Tick(_float fTimeDelta)
{



}









void CPipeScream::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_fDeltaTime += fTimeDelta;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}







HRESULT CPipeScream::Render()
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

	if (FAILED(m_pTextureCom0->Set_SRV(m_pShaderCom, "g_MaskAlpa", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom1->Set_SRV(m_pShaderCom, "g_Noise", 0)))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 4)))
			return E_FAIL;
	}



	return S_OK;
}







HRESULT CPipeScream::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("PipeScream"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_split_line"), TEXT("Com_Texture0"), (CComponent**)&m_pTextureCom0)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ColorNoise"), TEXT("Com_Texture1"), (CComponent**)&m_pTextureCom1)))
		return E_FAIL;

	return S_OK;
}






CPipeScream * CPipeScream::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPipeScream*		pInstance = new CPipeScream(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPipeScream"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPipeScream::Clone(void * pArg)
{
	CPipeScream*		pInstance = new CPipeScream(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPipeScream"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeScream::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom0);
	Safe_Release(m_pTextureCom1);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}

void CPipeScream::Tick_ImGui(_float fTimeDelta)
{
	ImGui::Begin("PipeScream");


	if (ImGui::Button("Replay"))
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vOriScale));


	ImGui::InputFloat("ScaleSpeed", &m_fScaleSpeed);

	_float3 vScale = m_pTransformCom->Get_Scale();
	if (ImGui::DragFloat3("Scale", (float*)&vScale))
		m_pTransformCom->Set_Scale(XMVectorSetW(XMLoadFloat3(&vScale), 1.f));

	_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (ImGui::DragFloat3("Pos", (float*)&vPos))
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, (XMVectorSetW(XMLoadFloat3(&vPos), 1.f)));

	ImGui::End();
}
