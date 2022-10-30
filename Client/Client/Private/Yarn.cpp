#include "stdafx.h"
#include "..\Public\Yarn.h"

#include "GameInstance.h"
#include "ToolManager.h"

#include "Player.h"

CYarn::CYarn(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CItem(pDevice, pContext)
{
	
}

CYarn::CYarn(const CYarn & rhs)
	: CItem(rhs)
{
	
}

HRESULT CYarn::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CYarn::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Item";


	return S_OK;
}

void CYarn::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	__super::Tick(fTimeDelta);



}

void CYarn::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	__super::LateTick(fTimeDelta);

	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld ,nullptr, nullptr);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_ITEM, this);
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CYarn::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if(CToolManager::Get_Instance()->Get_Debug())
		Render_Col();

	return S_OK;
}

HRESULT CYarn::SetUp_State(_fmatrix StateMatrix)
{
	if (FAILED(__super::SetUp_State(StateMatrix)))
		return E_FAIL;

	return S_OK;
}


void CYarn::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("StaticOBB", Desc.OtherDesc.sTag))
		{
			if (Get_Dead())
				return;

			CPlayer* pPlayer = (CPlayer*)Desc.pOther;

			pPlayer->Get_Item(m_InvenDesc);

			Set_Dead(true);
		}
	}
}






void CYarn::Use_Item()
{



}






HRESULT CYarn::Ready_Components()
{

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	strcpy(ColDesc.sTag, "Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	return S_OK;
}

CYarn * CYarn::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CYarn*		pInstance = new CYarn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CYarn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CYarn::Clone(void * pArg)
{
	CYarn*		pInstance = new CYarn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CYarn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYarn::Free()
{
	__super::Free();

}


