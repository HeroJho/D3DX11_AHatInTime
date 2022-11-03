#include "..\Public\Sphere.h"
#include "AABB.h"
#include "OBB.h"

CSphere::CSphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCollider(pDevice, pContext)
{

}

CSphere::CSphere(const CSphere & rhs)
	: CCollider(rhs)
{

}

HRESULT CSphere::Initialize_Prototype(CCollider::TYPE eColliderType)
{
	if (FAILED(__super::Initialize_Prototype(eColliderType)))
		return E_FAIL;

	m_eColliderType = CCollider::TYPE_SPHERE;

	return S_OK;
}

HRESULT CSphere::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pOriginal_Sphere = new BoundingSphere(m_ColliderDesc.vCenter, m_ColliderDesc.vSize.x);

	m_pSphere = new BoundingSphere(*m_pOriginal_Sphere);

	return S_OK;
}

void CSphere::Update(_fmatrix TransformMatrix, CNavigation* pNavi, CTransform* pTran, _float fMagicNum, _bool bIsInWisp)
{
	m_isColl = false;
	m_pOriginal_Sphere->Transform(*m_pSphere, TransformMatrix);

	if (nullptr != pNavi)
	{
		// 네비에 내 셀의 충돌체들을 가져와서 검사한다.

		// 충돌했다면 내 포스와 충돌체의 포스를 빼서 미는 방향을 구한다.
	}
}

_bool CSphere::Collision(CCollider * pTargetCollider)
{
	CCollider::TYPE		eType = pTargetCollider->Get_ColliderType();

	bool bIsColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		bIsColl = m_pSphere->Intersects(((CAABB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_OBB:
		bIsColl = m_pSphere->Intersects(((COBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_SPHERE:
		bIsColl = m_pSphere->Intersects(((CSphere*)pTargetCollider)->Get_Collider());
		break;
	}

	if (!m_isColl)
		m_isColl = bIsColl;

	if (bIsColl)
		pTargetCollider->SetbCol();

	return bIsColl;
}

HRESULT CSphere::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pBatch->Begin();	

	DX::Draw(m_pBatch, *m_pSphere, XMLoadFloat4(&m_vColor));

	m_pBatch->End();


	return S_OK;
}

CSphere * CSphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CCollider::TYPE eColliderType)
{
	CSphere*			pInstance = new CSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX(TEXT("Failed To Created : CSphere"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CSphere::Clone(void * pArg)
{
	CSphere*			pInstance = new CSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSphere"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CSphere::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginal_Sphere);
	Safe_Delete(m_pSphere);

}
