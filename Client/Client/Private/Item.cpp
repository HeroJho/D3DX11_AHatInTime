#include "stdafx.h"
#include "..\Public\Item.h"
#include "GameInstance.h"

CItem::CItem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
}

CItem::CItem(const CItem & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	ITEMDESC* Desc = (ITEMDESC*)pArg;

	lstrcpy(m_InvenDesc.szModelName, Desc->szModelName);
	m_InvenDesc.iCount = Desc->iCount;


	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransformCom->Set_Scale(XMLoadFloat3(&Desc->vScale));

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), Desc->vAngle.x
		, XMVectorSet(0.f, 1.f, 0.f, 0.f), Desc->vAngle.y
		, XMVectorSet(0.f, 0.f, 1.f, 0.f), Desc->vAngle.z);

	_vector vPos = XMLoadFloat3(&Desc->vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	return S_OK;
}

void CItem::Tick(_float fTimeDelta)
{

}

void CItem::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	RELEASE_INSTANCE(CGameInstance);
	if (true == isDraw)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	
}

HRESULT CItem::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix()));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}

void CItem::Update_ParentPos()
{

	//_matrix mParent = m_Sockets[i]->Get_OffSetMatrix()
	//	* m_Sockets[i]->Get_CombinedTransformation()
	//	* XMLoadFloat4x4(&m_mPivot)
	//	* pParentTran->Get_WorldMatrix();

	//SetUp_State(mParent);

}

HRESULT CItem::SetUp_State(_fmatrix StateMatrix)
{
	m_pParentTransformCom->Set_State(CTransform::STATE_RIGHT, StateMatrix.r[0]);
	m_pParentTransformCom->Set_State(CTransform::STATE_UP, StateMatrix.r[1]);
	m_pParentTransformCom->Set_State(CTransform::STATE_LOOK, StateMatrix.r[2]);
	m_pParentTransformCom->Set_State(CTransform::STATE_POSITION, StateMatrix.r[3]);

	m_pParentTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CItem::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_ParentTransform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_ParentTransform"), (CComponent**)&m_pParentTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_InvenDesc.szModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}



void CItem::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pParentTransformCom);
}
