#include "stdafx.h"
#include "..\Public\TimeObject.h"
#include "GameInstance.h"
#include  "ToolManager.h"

CTimeObject::CTimeObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTimeObject::CTimeObject(const CTimeObject & rhs)
	: CGameObject(rhs)
{
}






HRESULT CTimeObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTimeObject::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	TIMEOBJECTDESC* pDesc = (TIMEOBJECTDESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z, 1.f));

	m_pTransformCom->Set_Scale(XMVectorSetW(XMLoadFloat3(&pDesc->vScale), 1.f));

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), pDesc->vRotation.x
		, XMVectorSet(0.f, 1.f, 0.f, 0.f), pDesc->vRotation.y
		, XMVectorSet(0.f, 0.f, 1.f, 0.f), pDesc->vRotation.z);

	m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}

void CTimeObject::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);




}











void CTimeObject::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;


	// 여기서 셀에 있는 콜라이더와 충돌처리 확인하고 밀린다.
	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld);

	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		// if (MONSTER_DIE == m_eState)

	}



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_ITEM, this);

	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	if (true == isDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CTimeObject::Render()
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

	_bool bBlur = true;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bBlur", &bBlur, sizeof(_bool))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}


	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();


	return S_OK;
}

void CTimeObject::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

}







HRESULT CTimeObject::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("TimeObject"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));


	ColDesc.vCenter = _float3(0.f, 0.25f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.3f, 0.3f, 0.3f);
	strcpy(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	return S_OK;
}










CTimeObject * CTimeObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTimeObject*		pInstance = new CTimeObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTimeObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTimeObject::Clone(void * pArg)
{
	CTimeObject*		pInstance = new CTimeObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTimeObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimeObject::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
