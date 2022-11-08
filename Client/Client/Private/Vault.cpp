#include "stdafx.h"
#include "..\Public\Vault.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "ItemManager.h"
#include "CamManager.h"
#include "GameManager.h"

#include "Player.h"


CVault::CVault(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CItem(pDevice, pContext)
{

}

CVault::CVault(const CVault & rhs)
	: CItem(rhs)
{

}

HRESULT CVault::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CVault::Initialize(void * pArg)
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

	m_iNaviIndex = CToolManager::Get_Instance()->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return S_OK;
}

void CVault::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);
	__super::Tick(fTimeDelta);


	switch (m_eState)
	{
	case Client::CVault::STATE_IDLE:
		Idle_Tick(fTimeDelta);
		break;
	case Client::CVault::STATE_OPEN:
		Open_Tick(fTimeDelta);
		break;
	case Client::CVault::STATE_OPENED:
		Opened_Tick(fTimeDelta);
		break;
	}

}

void CVault::LateTick(_float fTimeDelta)
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

HRESULT CVault::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();

	return S_OK;
}

HRESULT CVault::SetUp_State(_fmatrix StateMatrix)
{
	if (FAILED(__super::SetUp_State(StateMatrix)))
		return E_FAIL;

	return S_OK;
}


void CVault::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
		{
				
			m_bStartOpenning = true;

			//CPlayer* pPlayer = (CPlayer*)Desc.pOther;

			//pPlayer->Get_Item(m_InvenDesc);

		}
	}

}

void CVault::Set_State(VAULT_STATE eState)
{
	m_ePreState = m_eState;
	m_eState = eState;


	if (m_ePreState != m_eState)
	{
		switch (m_eState)
		{
		//case Client::CMad_Crow::MONSTER_ATTACKED:
		//	break;
		default:
			break;
		}
	}

	Set_Anim();
}



void CVault::Set_Anim()
{
	switch (m_eState)
	{
	case Client::CVault::STATE_IDLE:
		m_pModelCom->Set_AnimIndex(0);
		break;
	case Client::CVault::STATE_OPEN:
		m_pModelCom->Set_AnimIndex(2);
		break;
	case Client::CVault::STATE_OPENED:
		m_pModelCom->Set_AnimIndex(1);
		break;
	}
}


 
void CVault::Idle_Tick(_float fTimeDelta)
{

	if (Check_OpenIf())
	{
		Set_State(STATE_OPEN);
		CCamManager::Get_Instance()->Play_CutScene(2, true, m_pTransformCom);
	}

}

void CVault::Open_Tick(_float fTimeDelta)
{

	m_fSprintItemTimeAcc += fTimeDelta;

	if (3.f < m_fSprintItemTimeAcc && !m_bIsSprintItem)
	{
		m_bIsSprintItem = true;
		_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CItemManager::Get_Instance()->Make_PopSprintItem(TEXT("Prototype_GameObject_Diamond"), TEXT("capsule"), LEVEL_GAMEPLAY, vPos, _float3(0.f, 0.f, 0.f), _float3(1.f, 1.f, 1.f), 1, m_iNaviIndex, 30);
		CItemManager::Get_Instance()->Make_PopSprintItem(TEXT("Prototype_GameObject_Yarn"), TEXT("yarn_ui_sprint"), LEVEL_GAMEPLAY, vPos, _float3(0.f, 0.f, 0.f), _float3(1.f, 1.f, 1.f), 1, m_iNaviIndex, 2);
		CGameManager::Get_Instance()->Set_JumpVault();
	}

}

void CVault::Opened_Tick(_float fTimeDelta)
{

}





void CVault::Use_Item()
{



}






HRESULT CVault::Ready_Components()
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

_bool CVault::Check_OpenIf()
{
	// 상자가 열리는 조건

	return m_bStartOpenning;
}

CVault * CVault::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVault*		pInstance = new CVault(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVault"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CVault::Clone(void * pArg)
{
	CVault*		pInstance = new CVault(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVault"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVault::Free()
{
	__super::Free();


}


