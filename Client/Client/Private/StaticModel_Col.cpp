#include "stdafx.h"
#include "..\Public\StaticModel_Col.h"
#include "GameInstance.h"

#include "ToolManager.h"


CStaticModel_Col::CStaticModel_Col(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CStaticModel_Col::CStaticModel_Col(const CStaticModel_Col & rhs)
	: CGameObject(rhs)
{

}


HRESULT CStaticModel_Col::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStaticModel_Col::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	STATICMODELDESC* Desc = (STATICMODELDESC*)pArg;
	wcscpy_s(m_cModelTag, Desc->cModelTag);


	if (FAILED(Ready_Components(Desc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&Desc->vPos), 1.f));
	
	_vector vAngle = XMLoadFloat3(&Desc->vAngle);
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMVectorGetX(vAngle),
		XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVectorGetY(vAngle),
		XMVectorSet(0.f, 0.f, 1.f, 0.f), XMVectorGetZ(vAngle));

	m_pTransformCom->Set_Scale(XMLoadFloat3(&Desc->vScale));


	// ���� ����Ѵ�
	m_pNavigationCom->Ready_CellCollision(this);

	return S_OK;
}

void CStaticModel_Col::Tick(_float fTimeDelta)
{
	Tick_Col(m_pTransformCom->Get_WorldMatrix());
}

void CStaticModel_Col::LateTick(_float fTimeDelta)
{
	
	if (nullptr == m_pRendererCom)
		return;
	if (CToolManager::Get_Instance()->Get_Debug())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CStaticModel_Col::Render()
{

	Render_Col();

	return S_OK;
}

HRESULT CStaticModel_Col::Ready_Components(STATICMODELDESC* Desc)
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;


	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom)))
		return E_FAIL;


	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = Desc->vCenter;
	ColDesc.vSize = Desc->vSize;
	ColDesc.vRotation = Desc->vRotation;
	ColDesc.bWall = Desc->bWall;
	ColDesc.bIsStatic = true;

	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;


	return S_OK;
}

CStaticModel_Col * CStaticModel_Col::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStaticModel_Col*		pInstance = new CStaticModel_Col(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStaticModel_Col"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStaticModel_Col::Clone(void * pArg)
{
	CStaticModel_Col*		pInstance = new CStaticModel_Col(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStaticModel_Col"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticModel_Col::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
