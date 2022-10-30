#include "stdafx.h"
#include "..\Public\Flask.h"

#include "GameInstance.h"
#include "ToolManager.h"

#include "Player.h"

#include "Flask_EX.h"

CFlask::CFlask(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CItem(pDevice, pContext)
{

}

CFlask::CFlask(const CFlask & rhs)
	: CItem(rhs)
{

}

HRESULT CFlask::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CFlask::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	ITEMDESC* ItemDesc = (ITEMDESC*)pArg;
	if (nullptr == ItemDesc->pDesc)
		return E_FAIL;
	FLASKDESC* FlaskDesc = (FLASKDESC*)(ItemDesc->pDesc);

	memcpy(&m_Desc, FlaskDesc, sizeof(FLASKDESC));


	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_sTag = "Tag_Item";

	m_pTransformCom->Jump(m_Desc.fJumpPow);

	return S_OK;
}

void CFlask::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	__super::Tick(fTimeDelta);

	if(!m_bOn)
		m_pTransformCom->Go_Dir(XMLoadFloat3(&m_Desc.vDir), m_Desc.fDirPow, fTimeDelta, m_pNavigationCom);
}

void CFlask::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	__super::LateTick(fTimeDelta);

	m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 0.7f);


	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(m_pTransformCom->Get_WorldMatrix(), m_pNavigationCom, m_pTransformCom);


	// ÂøÁö Çß´Ù¸é
	_float fTemp = 0.f;
	if (m_pNavigationCom->isGround(m_pTransformCom->Get_State(CTransform::STATE_POSITION), &fTemp) 
		|| COBB::COL_ON == Get_StaticOBB()->Get_ColState()
		|| COBB::COL_BLOCK == Get_StaticOBB()->Get_ColState())
	{
		m_bOn = true;

		CFlask_EX::FLASKEXDESC Desc;
		XMStoreFloat3(&Desc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CGameInstance::Get_Instance()->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Flask_EX"), LEVEL_GAMEPLAY, TEXT("Layer_EX"), &Desc);

		Set_Dead(true);
	}



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_ITEM, this);
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CFlask::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();

	return S_OK;
}

HRESULT CFlask::SetUp_State(_fmatrix StateMatrix)
{
	if (FAILED(__super::SetUp_State(StateMatrix)))
		return E_FAIL;

	return S_OK;
}


void CFlask::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	//if ("Tag_Player" == Desc.pOther->Get_Tag())
	//{
	//	if (!strcmp("StaticOBB", Desc.OtherDesc.sTag))
	//	{
	//		if (Get_Dead())
	//			return;

	//		CPlayer* pPlayer = (CPlayer*)Desc.pOther;

	//		pPlayer->Get_Item(m_InvenDesc);

	//		Set_Dead(true);
	//	}
	//}
}






void CFlask::Use_Item()
{



}






HRESULT CFlask::Ready_Components()
{


	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = m_Desc.iNaviIndex;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.2f, 0.2f, 0.2f);
	ColDesc.bIsStatic = true;
	strcpy(ColDesc.sTag, "Static_OBB");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;



	return S_OK;
}

CFlask * CFlask::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFlask*		pInstance = new CFlask(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFlask"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlask::Clone(void * pArg)
{
	CFlask*		pInstance = new CFlask(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFlask"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlask::Free()
{
	__super::Free();


	Safe_Release(m_pNavigationCom);

}


