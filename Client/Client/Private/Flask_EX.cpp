#include "stdafx.h"
#include "..\Public\Flask_EX.h"
#include "GameInstance.h"

CFlask_EX::CFlask_EX(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CFlask_EX::CFlask_EX(const CFlask_EX& rhs)
	: CGameObject(rhs)
{
}

HRESULT CFlask_EX::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFlask_EX::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	


	if (FAILED(Ready_Components()))
		return E_FAIL;

	FLASKEXDESC* Desc = (FLASKEXDESC*)pArg;




	//m_pTransformCom->Set_Scale(XMLoadFloat3(&Desc->vScale));

	//m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), Desc->vAngle.x
	//	, XMVectorSet(0.f, 1.f, 0.f, 0.f), Desc->vAngle.y
	//	, XMVectorSet(0.f, 0.f, 1.f, 0.f), Desc->vAngle.z);

	_vector vPos = XMLoadFloat3(&Desc->vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	return S_OK;
}

void CFlask_EX::Tick(_float fTimeDelta)
{

}

void CFlask_EX::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CFlask_EX::Render()
{
	//if (nullptr == m_pModelCom ||
	//	nullptr == m_pShaderCom)
	//	return E_FAIL;

	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	//if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix(), sizeof(_float4x4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	//	return E_FAIL;

	//RELEASE_INSTANCE(CGameInstance);



	//_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();


	//for (_uint i = 0; i < iNumMeshes; ++i)
	//{
	//	if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
	//		return E_FAIL;

	//	if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
	//		return E_FAIL;
	//}



	Render_Col();

	return S_OK;
}



HRESULT CFlask_EX::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, , TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	strcpy(ColDesc.sTag, "EX_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	return S_OK;
}



CFlask_EX * CFlask_EX::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFlask_EX*		pInstance = new CFlask_EX(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFlask_EX"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlask_EX::Clone(void * pArg)
{
	CFlask_EX*		pInstance = new CFlask_EX(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFlask_EX"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CFlask_EX::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
