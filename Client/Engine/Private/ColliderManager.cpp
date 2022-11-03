#include "..\Public\ColliderManager.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CColliderManager)

CColliderManager::CColliderManager()
{
}

void CColliderManager::Add_ColGroup(COLLIDERGROUP eGroup, CGameObject * pObj)
{
	if (nullptr == pObj)
		return;

	m_ObjGroups[eGroup].push_back(pObj);
	Safe_AddRef(pObj);
}

void CColliderManager::Calcul_ColGroup(COLLIDERGROUP eGroupL, COLLIDERGROUP eGroupR)
{
	for (auto& pObjL : m_ObjGroups[eGroupL])
	{
		for (auto& pObjR : m_ObjGroups[eGroupR])
		{
			if (pObjL == pObjR)
				continue;

			list<CCollider*>* LCols = pObjL->Get_Colliders();
			list<CCollider*>* RCols = pObjR->Get_Colliders();

			for (auto& pColL : *LCols)
			{
				for (auto& pColR : *RCols)
				{

					// ���� ������ ����� �浹�� �˻��Ѵ� -> �б��
					if ((COLLIDER_MONSTER == eGroupL && COLLIDER_MONSTER == eGroupR) && (CCollider::TYPE_SPHERE != pColL->Get_ColliderType() || CCollider::TYPE_SPHERE != pColR->Get_ColliderType()))
						continue;

					//// OBB �浹�� ����� �ʹ� ��δ� ! ��������.
					//if (CCollider::TYPE_OBB == pColL->Get_ColliderType() || CCollider::TYPE_OBB == pColR->Get_ColliderType())
					//	continue;


					if (pColL->Collision(pColR))
					{
						CCollider::OTHERTOMECOLDESC LDesc ,RDesc;
						LDesc.MyDesc = pColL->Get_Desc();
						LDesc.OtherDesc = pColR->Get_Desc();
						LDesc.pOther = pObjR;

						RDesc.MyDesc = pColR->Get_Desc();
						RDesc.OtherDesc = pColL->Get_Desc();
						RDesc.pOther = pObjL;

						pObjL->OnCollision(LDesc);
						pObjR->OnCollision(RDesc);
					}
				}
			}

		}
	}


}

void CColliderManager::Clear_ColGroup()
{

	for (_int i = 0; i < COLLIDER_END; ++i)
	{
		for (auto& pObj : m_ObjGroups[i])
			Safe_Release(pObj);
		m_ObjGroups[i].clear();
	}

}


void CColliderManager::Free()
{

	Clear_ColGroup();

}
