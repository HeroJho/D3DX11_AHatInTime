#include "stdafx.h"
#include "..\Public\Mad_Crow.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "DataManager.h"

#include "Player.h"

CMad_Crow::CMad_Crow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CMad_Crow::CMad_Crow(const CMad_Crow & rhs)
	: CMonster(rhs)
{
}






HRESULT CMad_Crow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMad_Crow::Initialize(void * pArg)
{
	__super::Initialize(pArg);


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Monster";

	list<ANIM_LINEAR_DATA> LinearDatas = CDataManager::Get_Instance()->Load_Anim("SubSpider");
	for (auto& Data : LinearDatas)
	{
		Set_AnimLinearData(Data);
	}


	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_CreatureDesc.vPos), 1.f));
	m_pTransformCom->Set_CurSpeed(1.f);

	Set_State(MONSTER_CHASE);

	return S_OK;
}







void CMad_Crow::Set_AnimLinearData(ANIM_LINEAR_DATA LinearData)
{
	m_pModelCom->Push_AnimLinearData(LinearData);
}

void CMad_Crow::Set_State(MONSTER_STATE eState)
{
	m_ePreState = m_eState;
	m_eState = eState;


	if (m_ePreState != m_eState)
	{
		switch (m_eState)
		{
		case Client::CMad_Crow::MONSTER_ATTACKED:
			break;
		}
	}

	Set_Anim();
}

void CMad_Crow::Set_Anim()
{

	switch (m_eState)
	{
	case Client::CMad_Crow::MONSTER_IDLE:
		m_pModelCom->Set_AnimIndex(2);
		break;
	case Client::CMad_Crow::MONSTER_MOVE:
		m_pModelCom->Set_AnimIndex(3);
		break;
	case Client::CMad_Crow::MONSTER_CHASE:
		m_pModelCom->Set_AnimIndex(3);
		break;
	case Client::CMad_Crow::MONSTER_ATTACKED:
		break;
	case Client::CMad_Crow::MONSTER_DIE:
		m_pModelCom->Set_AnimIndex(0);
		break;
	}

}



void CMad_Crow::Attacked(_int iAT)
{
	--m_CreatureDesc.iHP;
	if (0 >= m_CreatureDesc.iHP)
	{
		Set_State(MONSTER_DIE);
	}
	else
	{
		Set_State(MONSTER_ATTACKED);
		m_pTransformCom->ReSet_AttackedAnim();
	}

}






void CMad_Crow::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);

	switch (m_eState)
	{
	case Client::CMad_Crow::MONSTER_IDLE:
		Tick_Idle(fTimeDelta);
		break;
	case Client::CMad_Crow::MONSTER_MOVE:
		Tick_Move(fTimeDelta);
		break;
	case Client::CMad_Crow::MONSTER_CHASE:
		Tick_Chase(fTimeDelta);
		break;
	case Client::CMad_Crow::MONSTER_ATTACKED:
		Tick_Attacked(fTimeDelta);
		break;
	case Client::CMad_Crow::MONSTER_DIE:
		Tick_Die(fTimeDelta);
		break;
	}

}

void CMad_Crow::Tick_Idle(_float fTimeDelta)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pPlayer = pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
	if (nullptr == pPlayer)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pTran = (CTransform*)pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));

	_vector vPlayerPos = pTran->Get_State(CTransform::STATE_POSITION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fDis = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));
	if (5.f > fDis)
		Set_State(MONSTER_CHASE);



	RELEASE_INSTANCE(CGameInstance);



}

void CMad_Crow::Tick_Move(_float fTimeDelta)
{

}

void CMad_Crow::Tick_Chase(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pPlayer = pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
	if (nullptr == pPlayer)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pTran = (CTransform*)pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));


	// ���� Ȯ��
	{
		_vector vPlayerPos = pTran->Get_State(CTransform::STATE_POSITION);
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float fDis = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));
		if (8.f < fDis)
			Set_State(MONSTER_IDLE);
	}

	// �߰�
	{
		_vector vLookPos = pTran->Get_State(CTransform::STATE_POSITION);

		m_pTransformCom->MoveTarget_Lend(vLookPos, 1.f, fTimeDelta, m_pNavigationCom, 0.5f);
	}



	RELEASE_INSTANCE(CGameInstance);
}

void CMad_Crow::Tick_Attacked(_float fTimeDelta)
{

	if (m_pTransformCom->Tick_AttackAnim(fTimeDelta))
		Set_State(MONSTER_CHASE);

}

void CMad_Crow::Tick_Die(_float fTimeDelta)
{
	m_fDeadTimeAcc += fTimeDelta;
	if (10.f < m_fDeadTimeAcc)
		Set_Dead(true);

}




void CMad_Crow::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);

	if (nullptr == m_pRendererCom)
		return;

	m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom);

	// ���⼭ ���� �ִ� �ݶ��̴��� �浹ó�� Ȯ���ϰ� �и���.
	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld, m_pNavigationCom, m_pTransformCom);

	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		// if (MONSTER_DIE == m_eState)

	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);


	if (MONSTER_DIE != m_eState)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);
		RELEASE_INSTANCE(CGameInstance);
	}
}

HRESULT CMad_Crow::Render()
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


	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();


	return S_OK;
}

void CMad_Crow::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	if (!strcmp("Attacked_Sphere", Desc.MyDesc.sTag) && !strcmp("StaticOBB", Desc.OtherDesc.sTag))
	{
		CPlayer* pPlayer = (CPlayer*)Desc.pOther;

		if (CPlayer::STATE_JUMPATTACK != pPlayer->Get_State() &&
			CPlayer::STATE_JUMPATTACKRENDING != pPlayer->Get_State() &&
			false == pPlayer->Get_Attacked())
		{
			pPlayer->Attacked();
		}
	}

}














HRESULT CMad_Crow::Ready_Components()
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

	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.3f, 1.f, 0.3f);
	ColDesc.bIsStatic = true;
	strcpy(ColDesc.sTag, "StaticOBB");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	//ColDesc.vCenter = _float3(0.f, 0.2f, 0.f);
	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	//if (FAILED(AddCollider(CCollider::TYPE_AABB, ColDesc)))
	//	return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.25f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.3f, 0.3f, 0.3f);
	strcpy(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = m_CreatureDesc.iNaviIndex;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;



	return S_OK;
}










CMad_Crow * CMad_Crow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMad_Crow*		pInstance = new CMad_Crow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMad_Crow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMad_Crow::Clone(void * pArg)
{
	CMad_Crow*		pInstance = new CMad_Crow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMad_Crow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMad_Crow::Free()
{
	__super::Free();


	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
