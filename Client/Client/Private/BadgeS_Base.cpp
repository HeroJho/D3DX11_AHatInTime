#include "stdafx.h"
#include "..\Public\BadgeS_Base.h"
#include "GameInstance.h"
#include  "ToolManager.h"
#include "DataManager.h"
#include "UIManager.h"
#include "Player.h"

#include "IceBox.h"

CBadgeS_Base::CBadgeS_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBadgeS_Base::CBadgeS_Base(const CBadgeS_Base & rhs)
	: CGameObject(rhs)
{
}






HRESULT CBadgeS_Base::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBadgeS_Base::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	list<ANIM_LINEAR_DATA> LinearDatas = CDataManager::Get_Instance()->Load_Anim("BadgeS_Base");
	for (auto& Data : LinearDatas)
	{
		Set_AnimLinearData(Data);
	}

	m_sTag = "BageS_Base";

	BADGES_BASEDESC* pDesc = (BADGES_BASEDESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), pDesc->vAngle.x
		, XMVectorSet(0.f, 1.f, 0.f, 0.f), pDesc->vAngle.y
		, XMVectorSet(0.f, 0.f, 1.f, 0.f), pDesc->vAngle.z);


	m_pTransformCom->Set_RealOriScale(XMVectorSet(1.5f, 1.5f, 1.5f, 1.f));

	Set_State(STATE_IDLE);
	m_pModelCom->Play_Animation(0.1f);


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	
	CIceBox::ICEBOXDESC IceDesc;
	IceDesc.pOwner = this;
	IceDesc.vPos = pDesc->vPos;
	IceDesc.vScale = _float3(1.5f, 1.5f, 1.5f);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_IceBox"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &IceDesc)))
		return E_FAIL;
	
	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

void CBadgeS_Base::Tick(_float fTimeDelta)
{
	// TODO
	// fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_END);

	switch (m_eState)
	{
	case STATE_IDLE:
		Idle_Tick(fTimeDelta);
		break;
	case STATE_TALK:
		Talk_Tick(fTimeDelta);
		break;
	case STATE_OPEN:
		Open_Tick(fTimeDelta);
		break;
	case STATE_ATTACKED:
		Attacked_Tick(fTimeDelta);
		break;
	}

}



void CBadgeS_Base::Idle_Tick(_float fTimeDelta)
{
	if (nullptr == m_pPlayer)
		return;

	CTransform* pTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));

	_vector vPlayerPos = pTran->Get_State(CTransform::STATE_POSITION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDis = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));
	if (2.f > fDis)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (pGameInstance->Key_Down(DIK_E))
		{
			Set_State(STATE_OPEN);
			((CPlayer*)m_pPlayer)->Set_Talk();
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CBadgeS_Base::Talk_Tick(_float fTimeDelta)
{

	CTransform* pTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));

	_vector vPlayerPos = pTran->Get_State(CTransform::STATE_POSITION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDis = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));
	if (5.f < fDis)
		Set_State(STATE_IDLE);

}

void CBadgeS_Base::Open_Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_E))
	{
		Set_State(STATE_IDLE);
	}

	RELEASE_INSTANCE(CGameInstance);


	if (CPlayer::STATE_TALK != ((CPlayer*)m_pPlayer)->Get_State())
		Set_State(STATE_IDLE);

	CTransform* pTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));

	_vector vPlayerPos = pTran->Get_State(CTransform::STATE_POSITION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDis = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));
	if (5.f < fDis)
		Set_State(STATE_IDLE);

}

void CBadgeS_Base::Attacked_Tick(_float fTimeDelta)
{
	if (m_pTransformCom->Tick_AttackAnim(fTimeDelta))
		Set_State(STATE_IDLE);
}




void CBadgeS_Base::Attacked(_int iAT)
{
	Set_State(STATE_ATTACKED);
	m_pTransformCom->ReSet_AttackedAnim();
}







void CBadgeS_Base::LateTick(_float fTimeDelta)
{
	// fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);

	if (nullptr == m_pRendererCom)
		return;

	// 여기서 셀에 있는 콜라이더와 충돌처리 확인하고 밀린다.
	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld, nullptr, nullptr);


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_bIsIce)
	{
		m_pModelCom->Play_Animation(fTimeDelta);
		pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_NPC, this);
	}

	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	if (true == isDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CBadgeS_Base::Render()
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




void CBadgeS_Base::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	if ("Tag_Player" == Desc.pOther->Get_Tag() && !strcmp("Sphere", Desc.OtherDesc.sTag))
	{
		if (STATE_IDLE == m_eState)
		{
			m_pPlayer = Desc.pOther;
		}

	}

}











HRESULT CBadgeS_Base::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("BadgeS_Base"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.25f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	strcpy(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	///* For.Com_Navigation */
	//CNavigation::NAVIGATIONDESC NaviDesc;
	//ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	//NaviDesc.iCurrentIndex = 13444;
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	//	return E_FAIL;



	return S_OK;
}

void CBadgeS_Base::Set_State(STATE eState)
{
	// 상태 변할 때 한 번 실행 돼야하는 부분
	if (eState != m_eState)
	{
		switch (eState)
		{
		case Client::CBadgeS_Base::STATE_IDLE:
			CUIManager::Get_Instance()->Close_Shop();
			break;
		case Client::CBadgeS_Base::STATE_TALK:
			break;
		case Client::CBadgeS_Base::STATE_OPEN:
			CUIManager::Get_Instance()->Open_Shop();
			break;
		}
	}

	m_eState = eState;

	Set_Anim();
}

void CBadgeS_Base::Set_Anim()
{
	switch (m_eState)
	{
	case Client::CBadgeS_Base::STATE_IDLE:
		m_pModelCom->Set_AnimIndex(3);
		break;
	case Client::CBadgeS_Base::STATE_TALK:
		m_pModelCom->Set_AnimIndex(1);
		break;
	case Client::CBadgeS_Base::STATE_OPEN:
		// m_pModelCom->Set_AnimIndex(2);
		m_pModelCom->Set_AnimIndex(1);
		break;
	}
}

void CBadgeS_Base::Set_AnimLinearData(ANIM_LINEAR_DATA LinearData)
{
	m_pModelCom->Push_AnimLinearData(LinearData);
}








CBadgeS_Base * CBadgeS_Base::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBadgeS_Base*		pInstance = new CBadgeS_Base(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBadgeS_Base"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBadgeS_Base::Clone(void * pArg)
{
	CBadgeS_Base*		pInstance = new CBadgeS_Base(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBadgeS_Base"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBadgeS_Base::Free()
{
	__super::Free();


	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
