#include "stdafx.h"
#include "..\Public\Flask_EX.h"
#include "GameInstance.h"

#include "IceBox.h"

CFlask_EX::CFlask_EX(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CFlask_EX::CFlask_EX(const CFlask_EX& rhs)
	: CGameObject(rhs)
{
}

HRESULT CFlask_EX::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFlask_EX::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	


	if (FAILED(Ready_Components()))
		return E_FAIL;

	FLASKEXDESC* Desc = (FLASKEXDESC*)pArg;




	//m_pTransformCom->Set_Scale(XMLoadFloat3(&Desc->vScale));

	//m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), Desc->vAngle.x
	//	, XMVectorSet(0.f, 1.f, 0.f, 0.f), Desc->vAngle.y
	//	, XMVectorSet(0.f, 0.f, 1.f, 0.f), Desc->vAngle.z);

	_vector vPos = XMLoadFloat3(&Desc->vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	return S_OK;
}

void CFlask_EX::Tick(_float fTimeDelta)
{
	if (m_bTickAttack)
		Set_Dead(true);
}

void CFlask_EX::LateTick(_float fTimeDelta)
{
	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_PROJECT, this);
	RELEASE_INSTANCE(CGameInstance);

	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	m_bTickAttack = true;
}

HRESULT CFlask_EX::Render()
{


	Render_Col();

	return S_OK;
}



HRESULT CFlask_EX::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, , TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	strcpy(ColDesc.sTag, "EX_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	return S_OK;
}



void CFlask_EX::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if (!strcmp("EX_Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
	{
		
		if ("Tag_PuzzleCube" == Desc.pOther->Get_Tag())
		{
			CTransform* pTran = (CTransform*)Desc.pOther->Get_ComponentPtr(TEXT("Com_Transform"));
			pTran->Jump(9.f);
		}
		else if ("Tag_IceBox" == Desc.pOther->Get_Tag())
		{
			CIceBox* pIce = (CIceBox*)Desc.pOther;
			pIce->Attacked();
		}

		m_bTickAttack = true;
	}
}



CFlask_EX * CFlask_EX::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFlask_EX*		pInstance = new CFlask_EX(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFlask_EX"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlask_EX::Clone(void * pArg)
{
	CFlask_EX*		pInstance = new CFlask_EX(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFlask_EX"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CFlask_EX::Free()
{
	__super::Free();


	Safe_Release(m_pTransformCom);
}
