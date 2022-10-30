#include "stdafx.h"
#include "..\Public\RollingBarrel.h"
#include "GameInstance.h"

#include "ToolManager.h"

CRollingBarrel::CRollingBarrel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CRollingBarrel::CRollingBarrel(const CRollingBarrel & rhs)
	: CGameObject(rhs)
{
}






HRESULT CRollingBarrel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRollingBarrel::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Barrel";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-35.75, 13.f, 157.85, 1.f));
	m_pTransformCom->Set_CurSpeed(1.f);

	return S_OK;
}










void CRollingBarrel::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), 0.1f, fTimeDelta);
}



void CRollingBarrel::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;


	Tick_Col(m_pTransformCom->Get_WorldMatrix(), nullptr, nullptr);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CRollingBarrel::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}


	Render_Col();


	return S_OK;
}

void CRollingBarrel::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag() && !strcmp("StaticOBB", Desc.OtherDesc.sTag))
	{

	}
		// m_pOther = Desc.pOther;
}














HRESULT CRollingBarrel::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Mad_Crow"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));


	_float fAngle = 0.f;
	for (_uint i = 0; i < 8; ++i)
	{
		_matrix mRot = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fAngle));
		_vector vOriDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		_vector vCurDir = XMVector3TransformNormal(vOriDir, mRot);

		strcpy(ColDesc.sTag, "OBB");
		XMStoreFloat3(&ColDesc.vCenter, vCurDir * 0.9f);
		ColDesc.vRotation = _float3(0.f, 0.f, fAngle + 90.f);
		ColDesc.vSize = _float3(1.f, 0.5f, 3.f);
		if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
			return E_FAIL;

		fAngle += 360.f / 8.f;
	}





	return S_OK;
}










CRollingBarrel * CRollingBarrel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRollingBarrel*		pInstance = new CRollingBarrel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRollingBarrel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRollingBarrel::Clone(void * pArg)
{
	CRollingBarrel*		pInstance = new CRollingBarrel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRollingBarrel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRollingBarrel::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
