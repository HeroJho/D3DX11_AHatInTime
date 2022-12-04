#include "stdafx.h"
#include "..\Public\Vault_State.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "ItemManager.h"
#include "CamManager.h"
#include "GameManager.h"
#include "ParticleManager.h"

#include "Player.h"


CVault_State::CVault_State(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CItem(pDevice, pContext)
{

}

CVault_State::CVault_State(const CVault_State & rhs)
	: CItem(rhs)
{

}

HRESULT CVault_State::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CVault_State::Initialize(void * pArg)
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

void CVault_State::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);
	__super::Tick(fTimeDelta);

	if (15 > m_iSprintCount && m_Spr)
		Sprint_Tick(fTimeDelta);

	switch (m_eState)
	{
	case Client::CVault_State::STATE_IDLE:
		Idle_Tick(fTimeDelta);
		break;
	case Client::CVault_State::STATE_OPEN:
		Open_Tick(fTimeDelta);
		break;
	case Client::CVault_State::STATE_OPENED:
		Opened_Tick(fTimeDelta);
		break;
	}

}

void CVault_State::LateTick(_float fTimeDelta)
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

HRESULT CVault_State::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();

	return S_OK;
}

HRESULT CVault_State::SetUp_State(_fmatrix StateMatrix)
{
	if (FAILED(__super::SetUp_State(StateMatrix)))
		return E_FAIL;

	return S_OK;
}


void CVault_State::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
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

void CVault_State::Set_State(VAULT_STATE eState)
{
	m_ePreState = m_eState;
	m_eState = eState;


	if (m_ePreState != m_eState)
	{
		switch (m_eState)
		{
		case STATE_OPEN:
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->PlaySoundW(L"Metal_groan1.wav", SOUND_EFFECT, g_fEffectSound + 0.2f);
			RELEASE_INSTANCE(CGameInstance);
		}
		break;
		default:
			break;
		}
	}

	Set_Anim();
}



void CVault_State::Set_Anim()
{
	switch (m_eState)
	{
	case Client::CVault_State::STATE_IDLE:
		m_pModelCom->Set_AnimIndex(0);
		break;
	case Client::CVault_State::STATE_OPEN:
		m_pModelCom->Set_AnimIndex(2);
		break;
	case Client::CVault_State::STATE_OPENED:
		m_pModelCom->Set_AnimIndex(1);
		break;
	}
}



void CVault_State::Idle_Tick(_float fTimeDelta)
{

	if (Check_OpenIf())
	{
		Set_State(STATE_OPEN);
		CCamManager::Get_Instance()->Play_CutScene(2, true, m_pTransformCom);
	}

}

void CVault_State::Open_Tick(_float fTimeDelta)
{
	m_fSprintItemTimeAcc += fTimeDelta;

	if (3.f < m_fSprintItemTimeAcc && !m_bIsSprintItem)
	{
		m_bIsSprintItem = true;
		m_Spr = true;
		_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_Star"), vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(1.5f, 1.5f, 1.5f), _float3(0.5f, 0.5f, 0.5f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 3.f, 30.f, false, 3.f, 0.1f, 5.f,
			30, 0.f, 0.5f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, _float3(-90.f, 0.f, -90.f), _float3(90.f, 0.f, 90.f), CParticle::TYPE_TEXTURE);

		CItemManager::Get_Instance()->Make_PopSprintItem(TEXT("Prototype_GameObject_Yarn"), TEXT("yarn_ui_brew"), LEVEL_GAMEPLAY, vPos, _float3(0.f, 0.f, 0.f), _float3(1.f, 1.f, 1.f), 1, m_iNaviIndex, 1);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->PlaySoundW(L"DOTPLAT_DWN.wav", SOUND_EFFECT, g_fEffectSound + 0.2f);
		pGameInstance->PlaySoundW(L"Pickup_51.wav", SOUND_PEFFECT, g_fEffectSound + 0.4f);
		RELEASE_INSTANCE(CGameInstance);

		m_fSprintItemTimeAcc = 0.f;
		CGameManager::Get_Instance()->Get_Instance()->IncVir();
	}
}

void CVault_State::Opened_Tick(_float fTimeDelta)
{

}





void CVault_State::Use_Item()
{



}

void CVault_State::Sprint_Tick(_float fTimeDelta)
{
	m_fSprintItemTimeAcc += fTimeDelta;

	if (0.4f < m_fSprintItemTimeAcc)
	{
		_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		CItemManager::Get_Instance()->Make_PopSprintItem(TEXT("Prototype_GameObject_DiamondW"), TEXT("capsule"), LEVEL_GAMEPLAY, vPos, _float3(0.f, 0.f, 0.f), _float3(1.f, 1.f, 1.f), 1, m_iNaviIndex, 1);

		m_fSprintItemTimeAcc = 0.f;
		++m_iSprintCount;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->PlaySoundW(L"Bloop.mp3", SOUND_ITEM, g_fEffectSound + 1.f);
		RELEASE_INSTANCE(CGameInstance);
	}

}




HRESULT CVault_State::Ready_Components()
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

_bool CVault_State::Check_OpenIf()
{
	// 상자가 열리는 조건

	return m_bStartOpenning;
}

CVault_State * CVault_State::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVault_State*		pInstance = new CVault_State(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVault_State"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CVault_State::Clone(void * pArg)
{
	CVault_State*		pInstance = new CVault_State(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVault_State"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVault_State::Free()
{
	__super::Free();


}


