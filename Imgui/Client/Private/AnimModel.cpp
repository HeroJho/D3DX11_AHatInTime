#include "stdafx.h"
#include "..\Public\AnimModel.h"
#include "GameInstance.h"

#include "ParticleManager.h"

CAnimModel::CAnimModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
}

CAnimModel::CAnimModel(const CAnimModel & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
}

HRESULT CAnimModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimModel::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	ANIMMODELDESC* Desc = (ANIMMODELDESC*)pArg;

	wcscpy(m_cModelTag, Desc->cModelTag);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}


void CAnimModel::Set_AnimLinearData(ANIM_LINEAR_DATA LinearData)
{
	m_pModelCom->Push_AnimLinearData(LinearData);
}

void CAnimModel::Reset_AnimLinearData()
{
	m_pModelCom->Reset_AnimLinearData();
}

void CAnimModel::Set_AnimIndex(_uint iIndex)
{
	m_pModelCom->Set_AnimIndex(iIndex);
}



void CAnimModel::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_M))
	{
		//_float3 vPos;
		//XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		//_float3 vLook;
		//XMStoreFloat3(&vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		//_float3 vInvLook;
		//XMStoreFloat3(&vInvLook, -1.f * m_pTransformCom->Get_State(CTransform::STATE_LOOK));


		//CParticleManager::Get_Instance()->Create_Particle(TEXT("SmokeParticle"), vPos,
		//	_float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(1.f, 1.f, 1.f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f),
		//	vInvLook, 0.1f, 3.f, 
		//	false , 1.f, 1.f, 1.f);

		//CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), vPos,
		//	_float3(0.f, 0.f, 0.f), vInvLook, _float3(1.f, 1.f, 1.f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f),
		//	vInvLook, 0.1f, 3.f,
		//	true, 1.f, 1.f, 1.f
		//, CParticleManager::STATE_SIRCLE, 10, 0.f, 0.5f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f);

	}

	RELEASE_INSTANCE(CGameInstance);


}

void CAnimModel::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		if (m_pModelCom->Get_CurAnimIndex() == m_iStartAnimIndex)
		{
			m_pModelCom->Set_AnimIndex(m_iEndAnimIndex);
		}
		else if (m_pModelCom->Get_CurAnimIndex() == m_iEndAnimIndex)
		{
			m_pModelCom->Set_AnimIndex(m_iStartAnimIndex);
		}
	}


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CAnimModel::Render()
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



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}

	return S_OK;
}








HRESULT CAnimModel::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, m_cModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CAnimModel * CAnimModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAnimModel*		pInstance = new CAnimModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAnimModel::Clone(void * pArg)
{
	CAnimModel*		pInstance = new CAnimModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAnimModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimModel::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
