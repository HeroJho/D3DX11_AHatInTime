#include "stdafx.h"
#include "..\Public\MonsterVault.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "ItemManager.h"
#include "GameManager.h"
#include "CamManager.h"
#include "ParticleManager.h"

#include "Player.h"


CMonsterVault::CMonsterVault(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CItem(pDevice, pContext)
{

}

CMonsterVault::CMonsterVault(const CMonsterVault & rhs)
	: CItem(rhs)
{

}

HRESULT CMonsterVault::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CMonsterVault::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	ITEMDESC* Desc = (ITEMDESC*)pArg;

	lstrcpy(m_InvenDesc.szModelName, Desc->szModelName);
	m_InvenDesc.iCount = Desc->iCount;



	if (FAILED(Ready_Components()))
		return E_FAIL;


	// m_pTransformCom->Set_RealOriScale(XMVectorSet(2.f, 2.f, 2.f, 1.f));
	m_pTransformCom->Set_Scale(XMLoadFloat3(&Desc->vScale));

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), Desc->vAngle.x
		, XMVectorSet(0.f, 1.f, 0.f, 0.f), Desc->vAngle.y
		, XMVectorSet(0.f, 0.f, 1.f, 0.f), Desc->vAngle.z);

	_vector vPos = XMLoadFloat3(&Desc->vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	m_sTag = "Tag_Vault";
	Set_State(STATE_IDLE);


	// 몬스터 생성

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject::CREATUREINFODESC ObjDesc;
	ZeroMemory(&ObjDesc, sizeof(CGameObject::CREATUREINFODESC));
	ObjDesc.iAT = 1;
	ObjDesc.iMaxHP = 3;
	ObjDesc.iHP = 3;

	m_iNaviIndex = CToolManager::Get_Instance()->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_uint i = 0; i < 7; ++i)
	{
		XMStoreFloat3(&ObjDesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		ObjDesc.iNaviIndex = m_iNaviIndex;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Vault_Mad_Crow"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &ObjDesc)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

void CMonsterVault::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);
	__super::Tick(fTimeDelta);


	switch (m_eState)
	{
	case Client::CMonsterVault::STATE_IDLE:
		Idle_Tick(fTimeDelta);
		break;
	case Client::CMonsterVault::STATE_OPEN:
		Open_Tick(fTimeDelta);
		break;
	case Client::CMonsterVault::STATE_OPENED:
		Opened_Tick(fTimeDelta);
		break;
	}

}

void CMonsterVault::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	__super::LateTick(fTimeDelta);


	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		if (STATE_OPEN == m_eState)
		{
			Set_State(STATE_OPENED);
		}
	}


	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(m_pTransformCom->Get_WorldMatrix());


	if (STATE_OPENED != m_eState)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_ITEM, this);
		RELEASE_INSTANCE(CGameInstance);
	}

}

HRESULT CMonsterVault::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();

	return S_OK;
}

HRESULT CMonsterVault::SetUp_State(_fmatrix StateMatrix)
{
	if (FAILED(__super::SetUp_State(StateMatrix)))
		return E_FAIL;

	return S_OK;
}


void CMonsterVault::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("StaticOBB", Desc.OtherDesc.sTag))
		{

			m_bStartOpenning = true;

			//CPlayer* pPlayer = (CPlayer*)Desc.pOther;

			//pPlayer->Get_Item(m_InvenDesc);

		}
	}
}

void CMonsterVault::Set_State(VAULT_STATE eState)
{
	m_ePreState = m_eState;
	m_eState = eState;


	if (m_ePreState != m_eState)
	{
		switch (m_eState)
		{
			//case Client::CMad_Crow::MONSTER_ATTACKED:
			//	break;
		}
	}

	Set_Anim();
}



void CMonsterVault::Set_Anim()
{
	switch (m_eState)
	{
	case Client::CMonsterVault::STATE_IDLE:
		m_pModelCom->Set_AnimIndex(0);
		break;
	case Client::CMonsterVault::STATE_OPEN:
		m_pModelCom->Set_AnimIndex(2);
		break;
	case Client::CMonsterVault::STATE_OPENED:
		m_pModelCom->Set_AnimIndex(1);
		break;
	}
}



void CMonsterVault::Idle_Tick(_float fTimeDelta)
{

	if (Check_OpenIf())
	{
		Set_State(STATE_OPEN);
		CCamManager::Get_Instance()->Play_CutScene(2, true, m_pTransformCom);
	}

}

void CMonsterVault::Open_Tick(_float fTimeDelta)
{

	m_fSprintItemTimeAcc += fTimeDelta;

	if (3.f < m_fSprintItemTimeAcc && !m_bIsSprintItem)
	{
		m_bIsSprintItem = true;
		_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CItemManager::Get_Instance()->Make_PopSprintItem(TEXT("Prototype_GameObject_Diamond"), TEXT("capsule"), LEVEL_GAMEPLAY, vPos, _float3(0.f, 0.f, 0.f), _float3(1.f, 1.f, 1.f), 1, m_iNaviIndex, 15);
		
		CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_Star"), vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(1.5f, 1.5f, 1.5f), _float3(0.5f, 0.5f, 0.5f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 3.f, 30.f, false, 3.f, 0.1f, 5.f,
			30, 0.f, 0.5f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, _float3(-90.f, 0.f, -90.f), _float3(90.f, 0.f, 90.f), CParticle::TYPE_TEXTURE);

	}

}

void CMonsterVault::Opened_Tick(_float fTimeDelta)
{

}





void CMonsterVault::Use_Item()
{



}






HRESULT CMonsterVault::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_InvenDesc.szModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;





	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColDesc.bIsStatic = true;
	strcpy(ColDesc.sTag, "StaticOBB");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;


	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColDesc.bIsStatic = false;
	strcpy(ColDesc.sTag, "Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	return S_OK;
}

_bool CMonsterVault::Check_OpenIf()
{
	// 상자가 열리는 조건
	return 	CGameManager::Get_Instance()->Check_MonsterVaultCount();
}

CMonsterVault * CMonsterVault::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterVault*		pInstance = new CMonsterVault(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMonsterVault"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonsterVault::Clone(void * pArg)
{
	CMonsterVault*		pInstance = new CMonsterVault(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMonsterVault"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterVault::Free()
{
	__super::Free();


}


