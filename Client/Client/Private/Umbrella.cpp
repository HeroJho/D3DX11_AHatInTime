#include "stdafx.h"
#include "..\Public\Umbrella.h"
#include "GameInstance.h"
#include "Player.h"
#include "ToolManager.h"

CUmbrella::CUmbrella(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
}

CUmbrella::CUmbrella(const CUmbrella & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
}

HRESULT CUmbrella::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUmbrella::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CSockat::PARTSDESC* Desc = (CSockat::PARTSDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != Desc->pOwner)
	{
		m_pOwner = Desc->pOwner;
		//Safe_AddRef(m_pOwner);
	}


	m_pTransformCom->Set_Scale(XMLoadFloat3(&Desc->vScale));

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), Desc->vRot.x
		, XMVectorSet(0.f, 1.f, 0.f, 0.f), Desc->vRot.y
		, XMVectorSet(0.f, 0.f, 1.f, 0.f), Desc->vRot.z);

	_vector vPos = XMLoadFloat3(&Desc->vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);




	return S_OK;
}

void CUmbrella::Tick(_float fTimeDelta)
{
	CPlayer* pPlayer = ((CPlayer*)m_pOwner);
	CPlayer::STATE ePlayerCurState = pPlayer->Get_State();

	if (CPlayer::STATE_READYATTACK == ePlayerCurState)
	{
		m_fAttackTimeAcc = 0.f;
		m_bCanAttack = false;
	}

	if (CPlayer::STATE_ATTACK_1 == ePlayerCurState ||
		CPlayer::STATE_ATTACK_2 == ePlayerCurState ||
		CPlayer::STATE_ATTACK_3 == ePlayerCurState )
	{
		m_fAttackTimeAcc += fTimeDelta;
	}
	else
	{
		m_fAttackTimeAcc = 0.f;
		m_bCanAttack = false;
	}


	if (0.05f < m_fAttackTimeAcc && 0.2f > m_fAttackTimeAcc)
	{
		m_bCanAttack = true;
	}

}

void CUmbrella::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	// 여기서 셀에 있는 콜라이더와 충돌처리 확인하고 밀린다.
	_matrix		WorldMatrix = m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix();
	Tick_Col(WorldMatrix);


	if (m_bCanAttack)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_SWORD, this);
		RELEASE_INSTANCE(CGameInstance);
		m_bCanAttack = false;
	}
}

HRESULT CUmbrella::Render()
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

HRESULT CUmbrella::SetUp_State(_fmatrix StateMatrix)
{
	m_pParentTransformCom->Set_State(CTransform::STATE_RIGHT, StateMatrix.r[0]);
	m_pParentTransformCom->Set_State(CTransform::STATE_UP, StateMatrix.r[1]);
	m_pParentTransformCom->Set_State(CTransform::STATE_LOOK, StateMatrix.r[2]);
	m_pParentTransformCom->Set_State(CTransform::STATE_POSITION, StateMatrix.r[3]);

	m_pParentTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CUmbrella::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Umbrella"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.4f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.3f, 0.8f, 0.3f);
	ColDesc.bIsStatic = false;
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	//ColDesc.vCenter = _float3(0.f, 0.2f, 0.f);
	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	//if (FAILED(AddCollider(CCollider::TYPE_AABB, ColDesc)))
	//	return E_FAIL;

	//ColDesc.vCenter = _float3(0.f, 0.2f, 0.f);
	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	//if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
	//	return E_FAIL;

	return S_OK;
}

CUmbrella * CUmbrella::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUmbrella*		pInstance = new CUmbrella(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUmbrella"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUmbrella::Clone(void * pArg)
{
	CUmbrella*		pInstance = new CUmbrella(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUmbrella"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUmbrella::Free()
{
	__super::Free();


	// Safe_Release(m_pOwner);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pParentTransformCom);
}
