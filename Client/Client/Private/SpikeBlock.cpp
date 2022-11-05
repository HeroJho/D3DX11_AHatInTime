#include "stdafx.h"
#include "..\Public\SpikeBlock.h"
#include "GameInstance.h"

#include "GameManager.h"
#include "ToolManager.h"

CSpikeBlock::CSpikeBlock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSpikeBlock::CSpikeBlock(const CSpikeBlock & rhs)
	: CGameObject(rhs)
{
}






HRESULT CSpikeBlock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpikeBlock::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_Desc = *((SPIKEBLOCKDESC*)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Barrel";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_Desc.vPos.x, m_Desc.vPos.y, m_Desc.vPos.z, 1.f));
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_Desc.vScale));

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_Desc.vRotation.x
		, XMVectorSet(0.f, 1.f, 0.f, 0.f), m_Desc.vRotation.y
		, XMVectorSet(0.f, 0.f, 1.f, 0.f), m_Desc.vRotation.z);

	m_pTransformCom->Set_CurSpeed(1.f);

	return S_OK;
}










void CSpikeBlock::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);



	if (m_Desc.vMyRight)
	{
		_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		m_pTransformCom->Turn(vRight, m_Desc.fSpeed, fTimeDelta);
	}
	else
		m_pTransformCom->Turn(XMLoadFloat3(&m_Desc.vAix), m_Desc.fSpeed, fTimeDelta);
	
	if (nullptr != m_pOther)
	{
		CTransform* pTran = (CTransform*)m_pOther->Get_ComponentPtr(TEXT("Com_Transform"));
		CNavigation* pNavi = (CNavigation*)m_pOther->Get_ComponentPtr(TEXT("Com_Navigation"));
		_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		_vector vDir = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		pTran->Go_Dir(vDir, m_Desc.fSpeed, fTimeDelta, pNavi);
	}

}



void CSpikeBlock::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;


	Tick_Col(m_pTransformCom->Get_WorldMatrix(), nullptr, nullptr);



	if (CGameManager::Get_Instance()->Get_WispInfoNum())
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);
		RELEASE_INSTANCE(CGameInstance);
	}


	m_pOther = nullptr;
}

HRESULT CSpikeBlock::Render()
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


	_uint iPassIndex = 0;
	if (CGameManager::Get_Instance()->Get_WispInfoNum())
	{
		// iPassIndex = 3;

		//_float3 vWispPos = CGameManager::Get_Instance()->Get_WispPos();
		//_float fWispRatio = CGameManager::Get_Instance()->Get_WispRatio();

		//if (FAILED(m_pShaderCom->Set_RawValue("g_WispRatio", &fWispRatio, sizeof(_float))))
		//	return E_FAIL;
		//if (FAILED(m_pShaderCom->Set_RawValue("g_WispPos", &vWispPos, sizeof(_float3))))
		//	return E_FAIL;
		//_bool bWall = true;
		//if (FAILED(m_pShaderCom->Set_RawValue("g_Wall", &bWall, sizeof(_bool))))
		//	return E_FAIL;

	}
	else
		iPassIndex = 0;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}


	// Render_Col();


	return S_OK;
}

void CSpikeBlock::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag() && !strcmp("StaticOBB", Desc.OtherDesc.sTag) && !strcmp("StaticOBB", Desc.MyDesc.sTag))
		m_pOther = Desc.pOther;


}














HRESULT CSpikeBlock::Ready_Components()
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

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("secret_plain_rectangle"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	
	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = m_Desc.vColScale;
	ColDesc.bWall = true;
	strcpy(ColDesc.sTag, "StaticOBB");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(10.f, 10.f, 10.f);
	strcpy(ColDesc.sTag, "Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;




	return S_OK;
}










CSpikeBlock * CSpikeBlock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpikeBlock*		pInstance = new CSpikeBlock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSpikeBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpikeBlock::Clone(void * pArg)
{
	CSpikeBlock*		pInstance = new CSpikeBlock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSpikeBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpikeBlock::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
