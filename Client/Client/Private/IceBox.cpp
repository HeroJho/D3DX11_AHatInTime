#include "stdafx.h"
#include "..\Public\IceBox.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "GameManager.h"

#include "Player.h"
#include "BadgeS_Base.h"


CIceBox::CIceBox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CIceBox::CIceBox(const CIceBox & rhs)
	: CGameObject(rhs)
{
}






HRESULT CIceBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CIceBox::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	ICEBOXDESC* pDesc = (ICEBOXDESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Set_Scale(XMVectorSetW(XMLoadFloat3(&pDesc->vScale), 1.f));

	m_sTag = "Tag_IceBox";

	m_pOwner = (CBadgeS_Base*)pDesc->pOwner;


	return S_OK;
}







void CIceBox::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);


}








void CIceBox::LateTick(_float fTimeDelta)
{

	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



	// 여기서 셀에 있는 콜라이더와 충돌처리 확인하고 밀린다.
	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld);




	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_EM, this);


	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	if (true == isDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CIceBox::Render()
{
	if (nullptr == m_pIceModelCom ||
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



	_uint		iIceNumMeshes = m_pIceModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iIceNumMeshes; ++i)
	{
		if (FAILED(m_pIceModelCom->SetUp_OnShader(m_pShaderCom, m_pIceModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pIceModelCom->Render(m_pShaderCom, i, 5)))
			return E_FAIL;
	}




	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();


	return S_OK;
}

void CIceBox::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

}

void CIceBox::Attacked()
{
	m_pOwner->Set_Ice();
	Set_Dead(true);
}













HRESULT CIceBox::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("IceBlock"), TEXT("Com_IceModel"), (CComponent**)&m_pIceModelCom)))
		return E_FAIL;




	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 1.5f * 0.5f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColDesc.bIsStatic = true;
	ColDesc.bWall = false;
	strcpy(ColDesc.sTag, "StaticOBB");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.77f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	strcpy(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}










CIceBox * CIceBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CIceBox*		pInstance = new CIceBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CIceBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CIceBox::Clone(void * pArg)
{
	CIceBox*		pInstance = new CIceBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CIceBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIceBox::Free()
{
	__super::Free();


	Safe_Release(m_pIceModelCom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}



