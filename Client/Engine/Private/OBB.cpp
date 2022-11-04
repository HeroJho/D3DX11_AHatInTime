#include "..\Public\OBB.h"
#include "AABB.h"
#include "Sphere.h"

#include "Transform.h"
#include "Navigation.h"
#include "GameObject.h"

COBB::COBB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCollider(pDevice, pContext)
{

}

COBB::COBB(const COBB & rhs)
	: CCollider(rhs)
{

}

HRESULT COBB::Initialize_Prototype(CCollider::TYPE eColliderType)
{
	if (FAILED(__super::Initialize_Prototype(eColliderType)))
		return E_FAIL;

	m_eColliderType = CCollider::TYPE_OBB;

	return S_OK;
}

HRESULT COBB::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ZeroMemory(&m_vHillPoss, sizeof(_float3) * 3);

	/* 복제될때 셋팅하고자하는 상태로 갱신한다. */
	m_pOriginal_OBB = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(m_ColliderDesc.vSize.x * 0.5f,
		m_ColliderDesc.vSize.y * 0.5f,
		m_ColliderDesc.vSize.z * 0.5f), _float4(0.f, 0.f, 0.f, 1.f));

	/* 회전에 대한 상태. */
	_matrix		RotationMatrix = XMMatrixRotationX(XMConvertToRadians(m_ColliderDesc.vRotation.x)) *
		XMMatrixRotationY(XMConvertToRadians(m_ColliderDesc.vRotation.y)) *
		XMMatrixRotationZ(XMConvertToRadians((m_ColliderDesc.vRotation.z)));

	m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);

	m_pOriginal_OBB->Center = m_ColliderDesc.vCenter;

	m_pOBB = new BoundingOrientedBox(*m_pOriginal_OBB);

	return S_OK;
}




void COBB::Update(_fmatrix TransformMatrix, CNavigation* pNavi, CTransform* pTran, _float fMagicNum, _bool bIsInWisp)
{
	{
		//m_isColl = false;
		//m_pOriginal_OBB->Transform(*m_pOBB, TransformMatrix);

		//if (!m_bOnCollision)
		//{
		//	m_ePreColState = m_eColState;
		//	m_eColState = COL_NONE;
		//	ZeroMemory(&m_vHillPoss, sizeof(_float3) * 3);
		//}
		//m_bOnCollision = false;


		//if (nullptr == pNavi || nullptr == pTran || !m_ColliderDesc.bIsStatic)
		//	return;

		//// Cell 콜라이더를 가져온다.
		//const vector<CGameObject*>* pColliders = pNavi->Get_CurCellColliders();
		//_float fDis = 0.f;
		//_float3 vPushDir;
		//COBB* pStaticOBB = nullptr;
		//for (auto& pObj : *pColliders)
		//{

		//	CHECKSTATE Data;
		//	ZeroMemory(&Data, sizeof(CHECKSTATE));

		//	pStaticOBB = ((COBB*)pObj->Get_Colliders().front());

		//	// 충돌했따 -> 방향으로 민다
		//	if (pStaticOBB->Collision_OBB(this, pTran, &vPushDir, &fDis))
		//	{

		//		if (COL_ON == m_eColState)
		//		{
		//			Data.eState = COL_ON;

		//			// 점프를 했다면 면을 안 탄다.
		//			if (!(0.001f > pTran->Get_Velocity()))
		//			{
		//				Data.eState = COL_ON;
		//			}

		//		}
		//		else if (COL_DOWN == m_eColState)
		//		{
		//			Data.eState = COL_DOWN;
		//		}
		//		else if (COL_BLOCK == m_eColState)
		//		{
		//			Data.eState = COL_BLOCK;
		//		}
		//		else if (COL_SLIDE == m_eColState)
		//		{
		//			Data.eState = COL_SLIDE;
		//		}


		//		Data.fDis = fDis;
		//		Data.vPushDir = vPushDir;
		//		Data.pModel = pStaticOBB;

		//		if (pStaticOBB->Get_Desc().bWall)
		//			m_eTickWallColState.push_back(Data);
		//		else
		//			m_eTickColState.push_back(Data);

		//	}



		//}

		//if (!m_eTickColState.empty() || !m_eTickWallColState.empty())
		//	Compute_State(pNavi, pTran);
	}



	m_isColl = false;
	m_pOriginal_OBB->Transform(*m_pOBB, TransformMatrix);

	if (!m_bOnCollision)
	{
		m_ePreColState = m_eColState;
		m_eColState = COL_NONE;
		ZeroMemory(&m_vHillPoss, sizeof(_float3) * 3);
	}
	m_bOnCollision = false;


	if (nullptr == pNavi || nullptr == pTran || !m_ColliderDesc.bIsStatic)
		return;

	// Cell 콜라이더를 가져온다.
	const vector<CGameObject*>* pColliders = pNavi->Get_CurCellColliders();
	_float fDis = 0.f;
	_float3 vPushDir;
	COBB* pStaticOBB = nullptr;
	for (auto& pObj : *pColliders)
	{

		pStaticOBB = ((COBB*)pObj->Get_Colliders()->front());

		// 충돌했따 -> 방향으로 민다
		if (pStaticOBB->Collision_OBB(this, pTran, &vPushDir, &fDis))
		{

			// 웹 안에 있다
			if (bIsInWisp)
			{
				// 충돌체가 Wall아니면 노충돌
				if (!pStaticOBB->Get_Desc().bWall)
				{
					m_eColState = COL_NONE;
					continue;
				}
			}
			// 웹 밖에 있다
			else
			{
				// 충돌체가 Wall아니면 충돌
				if (pStaticOBB->Get_Desc().bWall)
				{
					m_eColState = COL_NONE;
					continue;
				}
			}



			if (COL_ON == m_eColState)
			{
				// 점프를 했다면 면을 안 탄다.
				if (0.01f < pTran->Get_Velocity())
					m_eColState = COL_NONE;
			}
			else if (COL_DOWN == m_eColState)
			{
				m_eColState = COL_DOWN;
			}
			else if (COL_BLOCK == m_eColState)
			{
				m_eColState = COL_BLOCK;
			}
			else if (COL_SLIDE == m_eColState)
			{
				m_eColState = COL_SLIDE;
			}


			// 충돌 연산 바로 진행

			_fvector vNorPushDir = XMVector3Normalize(XMLoadFloat3(&vPushDir));

			switch (m_eColState)
			{
			case Engine::COBB::COL_NONE:
				break;
			case Engine::COBB::COL_DOWN:
			{
				// 밑면과 부딪혔다면 그대로 떨어진다
				_vector vPreToCurDir = XMLoadFloat3(&pTran->Get_PrePos()) - pTran->Get_State(CTransform::STATE_POSITION);
				_float fPreToCurDis = fabs(XMVectorGetX(XMVector3Length(vPreToCurDir)));

				pTran->ResetGravity();
				pTran->Set_CurSpeed(0.f);
				pTran->Push_Dir(vNorPushDir, fPreToCurDis + 0.1f, pNavi);
			}
			break;
			case Engine::COBB::COL_BLOCK:			
			{
				// 벽과 부딪혔다면 그대로 밀어낸다
				_vector vPreToCurDir = XMLoadFloat3(&pTran->Get_PrePos()) - pTran->Get_State(CTransform::STATE_POSITION);
				_float fPreToCurDis = fabs(XMVectorGetX(XMVector3Length(vPreToCurDir)));

				// pTran->ResetGravity();
				pTran->Set_CurSpeed(0.f);

				// 충돌 풀릴때까지 조금씩 밀자..
				while (DontTouchMe(pStaticOBB) && 0.01f < (XMVectorGetX(XMVector3Length(vNorPushDir))))
				{
					pTran->Push_Dir(vNorPushDir, 0.01f, pNavi);
					m_pOriginal_OBB->Transform(*m_pOBB, pTran->Get_WorldMatrix());
				}
			}
			break;
			case Engine::COBB::COL_SLIDE:
			{
				pTran->ResetGravity();
				_vector vPos = pTran->Get_State(CTransform::STATE_POSITION);
				vPos = XMVectorSetY(vPos, fDis);
				pTran->Set_State(CTransform::STATE_POSITION, vPos);
			}
			break;
			case Engine::COBB::COL_ON:
			{
				pTran->ResetGravity();
				_vector vPos = pTran->Get_State(CTransform::STATE_POSITION);
				vPos = XMVectorSetY(vPos, fDis + fMagicNum);
				pTran->Set_State(CTransform::STATE_POSITION, vPos);
			}
			break;
			}

		}


	}

	


}

_bool COBB::Collision_OBB(CCollider * pTargetCollider, CTransform* pTran, _float3* Out_fPushDir, _float* Out_fPlanY, _bool bIsConpi)
{
	if (TYPE_OBB != pTargetCollider->Get_ColliderType())
		return false;
	if (!m_pOBB->Intersects(((COBB*)pTargetCollider)->Get_Collider()))
		return false;

	//if (!DontTouchMe(pTargetCollider))
	//	return false;

	OBBDESC			OBBDesc[2] = {
		Compute_OBBDesc(),
		((COBB*)pTargetCollider)->Compute_OBBDesc(),
	};

	// ===================================================================================================

	// 충돌했다! 어느 면으로 충돌했는지 체크한다.
	_vector vPushDir, vDownPushDir;
	ZeroMemory(&vPushDir, sizeof(_float3));
	ZeroMemory(&vDownPushDir, sizeof(_float3));

	// 교차한 면을 구한다.
	_float3			vPoints[8];
	m_pOBB->GetCorners(vPoints);


	_float3 vPlanDot = vPoints[2];

	for (_uint i = 0; i < 3; ++i)
	{
		_float3 vNorDir = OBBDesc[0].vAlignAxis[i];

		_float fD = -(vNorDir.x * vPlanDot.x) - (vNorDir.y * vPlanDot.y) - (vNorDir.z * vPlanDot.z);
		_vector vPlan = XMVectorSet(vNorDir.x, vNorDir.y, vNorDir.z, fD);

		PlaneIntersectionType PlanType = ((COBB*)pTargetCollider)->Get_Collider().Intersects(vPlan);

		if (INTERSECTING == PlanType)
		{
			//if (1 == i)
			//	vDownPushDir += XMLoadFloat3(&vNorDir);
			//else
				vPushDir += XMLoadFloat3(&vNorDir);
		}

	}
	vPlanDot = vPoints[4];
	for (_uint i = 0; i < 3; ++i)
	{
		_float3 vNorDir;
		XMStoreFloat3(&vNorDir, -1.f * XMLoadFloat3(&OBBDesc[0].vAlignAxis[i]));

		_float fD = -(vNorDir.x * vPlanDot.x) - (vNorDir.y * vPlanDot.y) - (vNorDir.z * vPlanDot.z);
		_vector vPlan = XMVectorSet(vNorDir.x, vNorDir.y, vNorDir.z, fD);

		PlaneIntersectionType PlanType = ((COBB*)pTargetCollider)->Get_Collider().Intersects(vPlan);

		if (INTERSECTING == PlanType)
		{
			//if (1 == i)
			//	vDownPushDir += XMLoadFloat3(&vNorDir);
			//else
				vPushDir += XMLoadFloat3(&vNorDir);
		}
	}




	_vector vPos = pTran->Get_State(CTransform::STATE_POSITION);
	// 레이를 밑으로 쏴본다
	_float3 vPoss[3];
	_float	fDot = 0.f;
	((COBB*)pTargetCollider)->Get_Collider().GetCorners(vPoints);
	_vector vPos0 = XMLoadFloat3(&vPoints[0]); // + XMVector3Normalize(vPos - XMLoadFloat3(&vPoints[0]));
	_vector vPos1 = XMLoadFloat3(&vPoints[1]); // + XMVector3Normalize(vPos - XMLoadFloat3(&vPoints[1]));
	_vector vPos4 = XMLoadFloat3(&vPoints[4]); // + XMVector3Normalize(vPos - XMLoadFloat3(&vPoints[4]));
	_vector vPos5 = XMLoadFloat3(&vPoints[5]); // + XMVector3Normalize(vPos - XMLoadFloat3(&vPoints[5]));
	XMStoreFloat3(&vPoints[0], vPos0);
	XMStoreFloat3(&vPoints[1], vPos1);
	XMStoreFloat3(&vPoints[4], vPos4);
	XMStoreFloat3(&vPoints[5], vPos5);

	// 내적 값이 제일 작은 애의 면 정보를 담는다. -> 문제다 y 값이 낮은 애를 가져온다?
	_bool bIsBottomHit = false;
	_float fY = FLT_MAX;
	if (Compute_LayPlane(vPos, XMVectorSetW(XMLoadFloat3(&vPoints[0]), 1.f), XMVectorSet(0.f, -1.f, 0.f, 0.f), vPoss, &fDot, &fY))
		bIsBottomHit = true;
	if (Compute_LayPlane(vPos, XMVectorSetW(XMLoadFloat3(&vPoints[1]), 1.f), XMVectorSet(0.f, -1.f, 0.f, 0.f), vPoss, &fDot, &fY))
		bIsBottomHit = true;
	if (Compute_LayPlane(vPos, XMVectorSetW(XMLoadFloat3(&vPoints[4]), 1.f), XMVectorSet(0.f, -1.f, 0.f, 0.f), vPoss, &fDot, &fY))
		bIsBottomHit = true;
	if (Compute_LayPlane(vPos, XMVectorSetW(XMLoadFloat3(&vPoints[5]), 1.f), XMVectorSet(0.f, -1.f, 0.f, 0.f), vPoss, &fDot, &fY))
		bIsBottomHit = true;
	

	if(bIsBottomHit)
	{// 맞았다
		// 레이에 맞은 면을 찾는다.
		// 너무 작은 면은 안 탄다 흘러 보낸다
		if (!Check_Area(XMLoadFloat3(&vPoss[0]), XMLoadFloat3(&vPoss[1]), XMLoadFloat3(&vPoss[2]), 0.5f))
		{
			((COBB*)pTargetCollider)->m_eColState = COL_NONE;
			return true;
		}

		// 해당 면을 탄다
		*Out_fPlanY = Compute_Height(vPos, vPoss);
		*Out_fPushDir = _float3(0.f, 0.f, 0.f);

		// 내적값이 0.7f 이하라면 (경사) 슬라이드
		// On이였는데 0.2f라면 None
		if (0.5f > fDot)
		{
			((COBB*)pTargetCollider)->m_eColState = COL_NONE;

			// 너무 가파르다 -> 벽이다
			if (0.2f > fDot)
			{

				if (COL_ON == ((COBB*)pTargetCollider)->m_ePreColState && !m_ColliderDesc.bWall)
				{
					((COBB*)pTargetCollider)->m_eColState = COL_NONE;
				}
				else
				{
 					_vector vAixY = XMVectorSet(0.f, 1.f, 0.f, 0.f);
					_vector vAixMyRight = XMVector3Cross(vAixY, vPushDir);
					vPushDir = XMVector3Cross(vAixMyRight, vAixY);
					((COBB*)pTargetCollider)->m_eColState = COL_BLOCK;
					XMStoreFloat3(Out_fPushDir, XMVector3Normalize(vPushDir));
				}

			}
			else
			{
				//if (COL_ON == ((COBB*)pTargetCollider)->m_ePreColState)
				//	((COBB*)pTargetCollider)->m_eColState = COL_NONE;
				//else
				if(!bIsConpi && COL_ON != ((COBB*)pTargetCollider)->m_eColState)
				{
					((COBB*)pTargetCollider)->m_eColState = COL_SLIDE;
					memcpy(((COBB*)pTargetCollider)->m_vHillPoss, vPoss, sizeof(_float3) * 3);
				}
				else if(bIsConpi)
				{
					((COBB*)pTargetCollider)->m_eColState = COL_SLIDE;
					memcpy(((COBB*)pTargetCollider)->m_vHillPoss, vPoss, sizeof(_float3) * 3);
				}
			}

		}
		else
		{
			((COBB*)pTargetCollider)->m_eColState = COL_ON;
		}

	}
	else
	{// 안 맞았다 -> 벽에 부딪혔거나 바닥 부딪힘.


		// 위로 레이를 쏴서 아무 충돌이 없다
		if (!Compute_LayPlane(XMVectorSet(0.f, 0.f, 0.f, 0.f), vPos, XMVectorSet(0.f, 1.f, 0.f, 0.f), vPoss, &fDot, nullptr))
		{ // 벽에 부딪혔다

			_vector vAixY = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector vAixMyRight = XMVector3Cross(vAixY, vPushDir);
			vPushDir = XMVector3Cross(vAixMyRight, vAixY);
			((COBB*)pTargetCollider)->m_eColState = COL_BLOCK;
			XMStoreFloat3(Out_fPushDir, XMVector3Normalize(vPushDir));
		}
		else
		{ // 바닥에 부딪혔다

			// 땅에 닿아있다면 x회전 없는 방향으로 민다
			// 땅에 안닿아있다면 면 방향으로 민다
			//     "   / <-    "
 			if (0.001f > pTran->Get_GravityAcc())
			{
				_vector vAixY = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				_vector vAixMyRight = XMVector3Cross(vAixY, vPushDir);
				vPushDir = XMVector3Cross(vAixMyRight, vAixY);
				((COBB*)pTargetCollider)->m_eColState = COL_BLOCK;
				XMStoreFloat3(Out_fPushDir, XMVector3Normalize(vPushDir));
			}
			else
			{
				XMStoreFloat3(Out_fPushDir, XMVector3Normalize(vPushDir));
				((COBB*)pTargetCollider)->m_eColState = COL_DOWN;
			}

		}

	}


	return true;
}

_bool COBB::DontTouchMe(CCollider * pTargetCollider)
{
	if (CCollider::TYPE_SPHERE == pTargetCollider->Get_ColliderType())
		return E_FAIL;

	m_isColl = false;
	// ((COBB*)pTargetCollider)->m_eColState = COL_NONE;

	OBBDESC			OBBDesc[2] = {
		Compute_OBBDesc(),
		((COBB*)pTargetCollider)->Compute_OBBDesc(),
	};



	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			_vector			vCenterDir = XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter);

			_float			fDistance[3] = { 0.0f };
			fDistance[0] = fabs(XMVectorGetX(XMVector3Dot(vCenterDir, XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			fDistance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			fDistance[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			if (fDistance[0] > fDistance[1] + fDistance[2])
				return m_isColl;

		}
	}
	
	m_isColl = true;

	return m_isColl;
}

void COBB::Compute_Pigi(CGameObject * pObj, CNavigation* pNavi, CTransform* pTran, _bool bIsInWisp)
{
	_float3 vPushDir;
	_float fDis = 0.f;
	if (pObj->Get_Colliders()->empty())
		return;

	for (auto& pOBB : *(pObj->Get_Colliders()))
	{
		if (TYPE_OBB != pOBB->Get_ColliderType())
			continue;

		CHECKSTATE Data;
		ZeroMemory(&Data, sizeof(CHECKSTATE));

		COBB* pStaticOBB = nullptr;
		pStaticOBB = ((COBB*)pOBB);


		// 충돌했따 -> 방향으로 민다
		if (pStaticOBB->Collision_OBB(this, pTran, &vPushDir, &fDis, true))
		{
			
			// 웹 안에 있다
			if (bIsInWisp)
			{
				// 충돌체가 Wall아니면 노충돌
				if (!pStaticOBB->Get_Desc().bWall)
				{
					m_eColState = COL_NONE;
					continue;
				}
			}
			// 웹 밖에 있다
			else
			{
				// 충돌체가 Wall아니면 충돌
				if (pStaticOBB->Get_Desc().bWall)
				{
					m_eColState = COL_NONE;
					continue;
				}
			}


			m_bOnCollision = true;

			if (COL_ON == m_eColState)
			{
				m_eColState = COL_ON;

				// 점프를 했다면 면을 안 탄다.
				if (!(0.001f > pTran->Get_Velocity()))
				{
					m_eColState = COL_ON;
				}

			}
			else if (COL_DOWN == m_eColState)
			{
				m_eColState = COL_DOWN;
			}
			else if (COL_BLOCK == m_eColState)
			{
				m_eColState = COL_BLOCK;
			}
			else if (COL_SLIDE == m_eColState)
			{
				m_eColState = COL_SLIDE;
			}



			// 충돌 연산 바로 진행

			_fvector vNorPushDir = XMVector3Normalize(XMLoadFloat3(&vPushDir));

			switch (m_eColState)
			{
			case Engine::COBB::COL_NONE:
				break;
			case Engine::COBB::COL_DOWN:
			{
				// 밑면과 부딪혔다면 그대로 떨어진다
				_vector vPreToCurDir = XMLoadFloat3(&pTran->Get_PrePos()) - pTran->Get_State(CTransform::STATE_POSITION);
				_float fPreToCurDis = fabs(XMVectorGetX(XMVector3Length(vPreToCurDir)));

				pTran->ResetGravity();
				pTran->Set_CurSpeed(0.f);
				pTran->Push_Dir(vNorPushDir, fPreToCurDis + 0.1f, pNavi);
			}
			break;
			case Engine::COBB::COL_BLOCK:
			{
				// 벽과 부딪혔다면 그대로 밀어낸다
				_vector vPreToCurDir = XMLoadFloat3(&pTran->Get_PrePos()) - pTran->Get_State(CTransform::STATE_POSITION);
				_float fPreToCurDis = fabs(XMVectorGetX(XMVector3Length(vPreToCurDir)));

				// pTran->ResetGravity();
				pTran->Set_CurSpeed(0.f);

				// 충돌 풀릴때까지 조금씩 밀자..
				while (DontTouchMe(pStaticOBB) && 0.01f < (XMVectorGetX(XMVector3Length(vNorPushDir))))
				{
					pTran->Push_Dir(vNorPushDir, 0.01f, pNavi);
					m_pOriginal_OBB->Transform(*m_pOBB, pTran->Get_WorldMatrix());
				}
			}
			break;
			case Engine::COBB::COL_SLIDE:
			{
				pTran->ResetGravity();
				_vector vPos = pTran->Get_State(CTransform::STATE_POSITION);
				vPos = XMVectorSetY(vPos, fDis);
				pTran->Set_State(CTransform::STATE_POSITION, vPos);
			}
			break;
			case Engine::COBB::COL_ON:
			{
				pTran->ResetGravity();
				_vector vPos = pTran->Get_State(CTransform::STATE_POSITION);
				vPos = XMVectorSetY(vPos, fDis);
				pTran->Set_State(CTransform::STATE_POSITION, vPos);
			}
			break;
			}

		}
	}

}


_bool COBB::Collision(CCollider * pTargetCollider)
{
	CCollider::TYPE		eType = pTargetCollider->Get_ColliderType();

	bool bIsColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		bIsColl = m_pOBB->Intersects(((CAABB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_OBB:
		bIsColl = m_pOBB->Intersects(((COBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_SPHERE:
		bIsColl = m_pOBB->Intersects(((CSphere*)pTargetCollider)->Get_Collider());
		break;
	}

	if (!m_isColl)
		m_isColl = bIsColl;

	if (bIsColl)
		pTargetCollider->SetbCol();

	return bIsColl;
}

_bool COBB::Collision_Cell(_fvector vA, _fvector vB, _fvector vC, _fmatrix TransformMatrix)
{
	// Ver.1
	// 각도가 크게 휘면 검출 못 함.
	{
		//_vector vZeroA = XMVectorSetY(vA, 0.f);
		//_vector vZeroB = XMVectorSetY(vB, 0.f);
		//_vector vZeroC = XMVectorSetY(vC, 0.f);


		//_matrix mZeroMatrix = TransformMatrix;
		//mZeroMatrix.r[3] = XMVectorSetY(mZeroMatrix.r[3], 0.f);


		//_float3 vCenterTemp = m_pOriginal_OBB->Center;

		// m_pOriginal_OBB->Center = _float3(vCenterTemp.x, 0.f, vCenterTemp.z);
		///* 회전에 대한 상태. */
		//_matrix		RotationMatrix = XMMatrixRotationX(XMConvertToRadians(m_ColliderDesc.vRotation.x)) *
		//	XMMatrixRotationY(XMConvertToRadians(m_ColliderDesc.vRotation.y)) *
		//	XMMatrixRotationZ(XMConvertToRadians(m_ColliderDesc.vRotation.z));

		//RotationMatrix = XMMatrixInverse(nullptr, RotationMatrix);
		//m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);



		//m_pOriginal_OBB->Transform(*m_pOBB, mZeroMatrix);

		//RotationMatrix = XMMatrixInverse(nullptr, RotationMatrix);
		//m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);
		//m_pOriginal_OBB->Center = vCenterTemp;

		// return m_pOBB->Intersects(vZeroA, vZeroB, vZeroC);
	}

	
	// Ver.2
	// 평면 체크는 무한하다는 것 때문에 y를 투영해서는 불가능.
	{
		//if (!Check_Area(vA, vB, vC))
		//	return false;

		//m_pOriginal_OBB->Transform(*m_pOBB, TransformMatrix);


		//_float3			vPoints[8];
		//m_pOBB->GetCorners(vPoints);

		//// 쎌의 평면 상의 점을 구한다.
		//_vector vPlane = XMPlaneFromPoints(vA, vB, vC);

		//_float fa = XMVectorGetX(vPlane);
		//_float fb = XMVectorGetY(vPlane);
		//_float fc = XMVectorGetZ(vPlane);
		//_float fd = XMVectorGetW(vPlane);

		//_float3 vCellPos;
		//_vector vCA, vCB, vCC;
		//// 201  
		//vCellPos = Copute_CellY(vPoints[2], fa, fb, fc, fd);
		//vCA = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[0], fa, fb, fc, fd);
		//vCB = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[1], fa, fb, fc, fd);
		//vCC = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);

		//vCA = XMVectorSetY(vCA, XMVectorGetY(vCA) + 0.1f);
		//vCC = XMVectorSetY(vCC, XMVectorGetY(vCC) - 0.1f);
		//// 면 충돌 체크한다
		//if (Check_Area(vCA, vCB, vCC))
		//{
		//	PlaneIntersectionType eType = TriangleTests::Intersects(vCA, vCB, vCC, vPlane);

		//	if (INTERSECTING == eType)
		//		return true;
		//}

		//// 201230
		//vCellPos = Copute_CellY(vPoints[2], fa, fb, fc, fd);
		//vCA = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[3], fa, fb, fc, fd);
		//vCB = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[0], fa, fb, fc, fd);
		//vCC = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);

		//vCA = XMVectorSetY(vCA, XMVectorGetY(vCA) + 0.1f);
		//vCC = XMVectorSetY(vCC, XMVectorGetY(vCC) - 0.1f);
		//// 면 충돌 체크한다
		//if (Check_Area(vCA, vCB, vCC))
		//{
		//	PlaneIntersectionType eType = TriangleTests::Intersects(vCA, vCB, vCC, vPlane);

		//	if (INTERSECTING == eType)
		//		return true;
		//}
		//	
		//// 201230273 
		//vCellPos = Copute_CellY(vPoints[2], fa, fb, fc, fd);
		//vCA = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[7], fa, fb, fc, fd);
		//vCB = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[3], fa, fb, fc, fd);
		//vCC = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);

		//vCA = XMVectorSetY(vCA, XMVectorGetY(vCA) + 0.1f);
		//vCC = XMVectorSetY(vCC, XMVectorGetY(vCC) - 0.1f);
		//// 면 충돌 체크한다
		//if (Check_Area(vCA, vCB, vCC))
		//{
		//	PlaneIntersectionType eType = TriangleTests::Intersects(vCA, vCB, vCC, vPlane);

		//	if (INTERSECTING == eType)
		//		return true;
		//}
		//
		//// 201230273267
		//vCellPos = Copute_CellY(vPoints[2], fa, fb, fc, fd);
		//vCA = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[6], fa, fb, fc, fd);
		//vCB = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[7], fa, fb, fc, fd);
		//vCC = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);

		//vCA = XMVectorSetY(vCA, XMVectorGetY(vCA) + 0.1f);
		//vCC = XMVectorSetY(vCC, XMVectorGetY(vCC) - 0.1f);
		//// 면 충돌 체크한다
		//if (Check_Area(vCA, vCB, vCC))
		//{
		//	PlaneIntersectionType eType = TriangleTests::Intersects(vCA, vCB, vCC, vPlane);

		//	if (INTERSECTING == eType)
		//		return true;
		//}

		//// 201230273267654
		//vCellPos = Copute_CellY(vPoints[6], fa, fb, fc, fd);
		//vCA = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[5], fa, fb, fc, fd);
		//vCB = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[4], fa, fb, fc, fd);
		//vCC = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);

		//vCA = XMVectorSetY(vCA, XMVectorGetY(vCA) + 0.1f);
		//vCC = XMVectorSetY(vCC, XMVectorGetY(vCC) - 0.1f);
		//// 면 충돌 체크한다
		//if (Check_Area(vCA, vCB, vCC))
		//{
		//	PlaneIntersectionType eType = TriangleTests::Intersects(vCA, vCB, vCC, vPlane);

		//	if (INTERSECTING == eType)
		//		return true;
		//}

		//// 201230273267654647
		//vCellPos = Copute_CellY(vPoints[6], fa, fb, fc, fd);
		//vCA = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[4], fa, fb, fc, fd);
		//vCB = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[7], fa, fb, fc, fd);
		//vCC = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);

		//vCA = XMVectorSetY(vCA, XMVectorGetY(vCA) + 0.1f);
		//vCC = XMVectorSetY(vCC, XMVectorGetY(vCC) - 0.1f);
		//// 면 충돌 체크한다
		//if (Check_Area(vCA, vCB, vCC))
		//{
		//	PlaneIntersectionType eType = TriangleTests::Intersects(vCA, vCB, vCC, vPlane);

		//	if (INTERSECTING == eType)
		//		return true;
		//}

		//// 201230273267654647154
		//vCellPos = Copute_CellY(vPoints[1], fa, fb, fc, fd);
		//vCA = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[5], fa, fb, fc, fd);
		//vCB = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
		//vCellPos = Copute_CellY(vPoints[4], fa, fb, fc, fd);
		//vCC = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);

		//vCA = XMVectorSetY(vCA, XMVectorGetY(vCA) + 0.1f);
		//vCC = XMVectorSetY(vCC, XMVectorGetY(vCC) - 0.1f);
		//// 면 충돌 체크한다
		//if (Check_Area(vCA, vCB, vCC))
		//{
		//	PlaneIntersectionType eType = TriangleTests::Intersects(vCA, vCB, vCC, vPlane);

		//	if (INTERSECTING == eType)
		//		return true;
		//}

//// 201230273267654647154140
//vCellPos = Copute_CellY(vPoints[1], fa, fb, fc, fd);
//vCA = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
//vCellPos = Copute_CellY(vPoints[4], fa, fb, fc, fd);
//vCB = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
//vCellPos = Copute_CellY(vPoints[0], fa, fb, fc, fd);
//vCC = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);

//vCA = XMVectorSetY(vCA, XMVectorGetY(vCA) + 0.1f);
//vCC = XMVectorSetY(vCC, XMVectorGetY(vCC) - 0.1f);
//// 면 충돌 체크한다
//if (Check_Area(vCA, vCB, vCC))
//{
//	PlaneIntersectionType eType = TriangleTests::Intersects(vCA, vCB, vCC, vPlane);

//	if (INTERSECTING == eType)
//		return true;
//}

//// 201230273267654647154140374
//vCellPos = Copute_CellY(vPoints[3], fa, fb, fc, fd);
//vCA = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
//vCellPos = Copute_CellY(vPoints[7], fa, fb, fc, fd);
//vCB = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
//vCellPos = Copute_CellY(vPoints[4], fa, fb, fc, fd);
//vCC = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);

//vCA = XMVectorSetY(vCA, XMVectorGetY(vCA) + 0.1f);
//vCC = XMVectorSetY(vCC, XMVectorGetY(vCC) - 0.1f);
//// 면 충돌 체크한다
//if (Check_Area(vCA, vCB, vCC))
//{
//	PlaneIntersectionType eType = TriangleTests::Intersects(vCA, vCB, vCC, vPlane);

//	if (INTERSECTING == eType)
//		return true;
//}

//// 201230273267654647154140374340
//vCellPos = Copute_CellY(vPoints[3], fa, fb, fc, fd);
//vCA = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
//vCellPos = Copute_CellY(vPoints[4], fa, fb, fc, fd);
//vCB = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
//vCellPos = Copute_CellY(vPoints[0], fa, fb, fc, fd);
//vCC = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);

//vCA = XMVectorSetY(vCA, XMVectorGetY(vCA) + 0.1f);
//vCC = XMVectorSetY(vCC, XMVectorGetY(vCC) - 0.1f);
//// 면 충돌 체크한다
//if (Check_Area(vCA, vCB, vCC))
//{
//	PlaneIntersectionType eType = TriangleTests::Intersects(vCA, vCB, vCC, vPlane);

//	if (INTERSECTING == eType)
//		return true;
//}

//// 201230273267654647154140374340215
//vCellPos = Copute_CellY(vPoints[2], fa, fb, fc, fd);
//vCA = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
//vCellPos = Copute_CellY(vPoints[1], fa, fb, fc, fd);
//vCB = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
//vCellPos = Copute_CellY(vPoints[5], fa, fb, fc, fd);
//vCC = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);

//vCA = XMVectorSetY(vCA, XMVectorGetY(vCA) + 0.1f);
//vCC = XMVectorSetY(vCC, XMVectorGetY(vCC) - 0.1f);
//// 면 충돌 체크한다
//if (Check_Area(vCA, vCB, vCC))
//{
//	PlaneIntersectionType eType = TriangleTests::Intersects(vCA, vCB, vCC, vPlane);

//	if (INTERSECTING == eType)
//		return true;
//}

//// 256 
//vCellPos = Copute_CellY(vPoints[2], fa, fb, fc, fd);
//vCA = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
//vCellPos = Copute_CellY(vPoints[5], fa, fb, fc, fd);
//vCB = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);
//vCellPos = Copute_CellY(vPoints[6], fa, fb, fc, fd);
//vCC = XMVectorSetW(XMLoadFloat3(&vCellPos), 1.f);

//vCA = XMVectorSetY(vCA, XMVectorGetY(vCA) + 0.1f);
//vCC = XMVectorSetY(vCC, XMVectorGetY(vCC) - 0.1f);
//// 면 충돌 체크한다
//if (Check_Area(vCA, vCB, vCC))
//{
//	PlaneIntersectionType eType = TriangleTests::Intersects(vCA, vCB, vCC, vPlane);

//	if (INTERSECTING == eType)
//		return true;
//}

	}


	// Ver.3
	{
		m_pOriginal_OBB->Transform(*m_pOBB, TransformMatrix);


		_float3			vPoints[8];
		m_pOBB->GetCorners(vPoints);

		// 셀의 중점에서 가장 먼 점과의 거리를 알아낸다.
		_float fCellRad = 0.f;
		_float3 vCellSentor = Compute_MaxRad(vA, vB, vC, &fCellRad);
		vCellSentor.y = 0.f;

		// OBB 면의 ""
		// 201 
		_float fOBBRad = 0.f;
		_float3 vOBBSentor = Compute_MaxRad(XMLoadFloat3(&vPoints[2]), 
											XMLoadFloat3(&vPoints[0]), 
											XMLoadFloat3(&vPoints[1]), &fOBBRad);
		vOBBSentor.y = 0.f;

		_float fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCellSentor) - XMLoadFloat3(&vOBBSentor)));
		if (fCellRad + fOBBRad > fDis)
			return true;

		// 230 
		fOBBRad = 0.f;
		vOBBSentor = Compute_MaxRad(XMLoadFloat3(&vPoints[2]), 
									XMLoadFloat3(&vPoints[3]), 
									XMLoadFloat3(&vPoints[0]), &fOBBRad);
		vOBBSentor.y = 0.f;

		fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCellSentor) - XMLoadFloat3(&vOBBSentor)));
		if (fCellRad + fOBBRad > fDis)
			return true;

		// 273 
		fOBBRad = 0.f;
		vOBBSentor = Compute_MaxRad(XMLoadFloat3(&vPoints[2]),
			XMLoadFloat3(&vPoints[7]),
			XMLoadFloat3(&vPoints[3]), &fOBBRad);
		vOBBSentor.y = 0.f;

		fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCellSentor) - XMLoadFloat3(&vOBBSentor)));
		if (fCellRad + fOBBRad > fDis)
			return true;

		// 267 
		fOBBRad = 0.f;
		vOBBSentor = Compute_MaxRad(XMLoadFloat3(&vPoints[2]),
			XMLoadFloat3(&vPoints[6]),
			XMLoadFloat3(&vPoints[7]), &fOBBRad);
		vOBBSentor.y = 0.f;

		fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCellSentor) - XMLoadFloat3(&vOBBSentor)));
		if (fCellRad + fOBBRad > fDis)
			return true;

		// 654 
		fOBBRad = 0.f;
		vOBBSentor = Compute_MaxRad(XMLoadFloat3(&vPoints[6]),
			XMLoadFloat3(&vPoints[5]),
			XMLoadFloat3(&vPoints[4]), &fOBBRad);
		vOBBSentor.y = 0.f;

		fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCellSentor) - XMLoadFloat3(&vOBBSentor)));
		if (fCellRad + fOBBRad > fDis)
			return true;

		// 647 
		fOBBRad = 0.f;
		vOBBSentor = Compute_MaxRad(XMLoadFloat3(&vPoints[6]),
			XMLoadFloat3(&vPoints[4]),
			XMLoadFloat3(&vPoints[7]), &fOBBRad);
		vOBBSentor.y = 0.f;

		fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCellSentor) - XMLoadFloat3(&vOBBSentor)));
		if (fCellRad + fOBBRad > fDis)
			return true;

		// 154 
		fOBBRad = 0.f;
		vOBBSentor = Compute_MaxRad(XMLoadFloat3(&vPoints[1]),
			XMLoadFloat3(&vPoints[5]),
			XMLoadFloat3(&vPoints[4]), &fOBBRad);
		vOBBSentor.y = 0.f;

		fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCellSentor) - XMLoadFloat3(&vOBBSentor)));
		if (fCellRad + fOBBRad > fDis)
			return true;

		// 140 
		fOBBRad = 0.f;
		vOBBSentor = Compute_MaxRad(XMLoadFloat3(&vPoints[1]),
			XMLoadFloat3(&vPoints[4]),
			XMLoadFloat3(&vPoints[0]), &fOBBRad);
		vOBBSentor.y = 0.f;

		fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCellSentor) - XMLoadFloat3(&vOBBSentor)));
		if (fCellRad + fOBBRad > fDis)
			return true;
		
		// 374 
		fOBBRad = 0.f;
		vOBBSentor = Compute_MaxRad(XMLoadFloat3(&vPoints[3]),
			XMLoadFloat3(&vPoints[7]),
			XMLoadFloat3(&vPoints[4]), &fOBBRad);
		vOBBSentor.y = 0.f;

		fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCellSentor) - XMLoadFloat3(&vOBBSentor)));
		if (fCellRad + fOBBRad > fDis)
			return true;

		// 340 
		fOBBRad = 0.f;
		vOBBSentor = Compute_MaxRad(XMLoadFloat3(&vPoints[3]),
			XMLoadFloat3(&vPoints[4]),
			XMLoadFloat3(&vPoints[0]), &fOBBRad);
		vOBBSentor.y = 0.f;

		fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCellSentor) - XMLoadFloat3(&vOBBSentor)));
		if (fCellRad + fOBBRad > fDis)
			return true;

		// 215 
		fOBBRad = 0.f;
		vOBBSentor = Compute_MaxRad(XMLoadFloat3(&vPoints[2]),
			XMLoadFloat3(&vPoints[1]),
			XMLoadFloat3(&vPoints[5]), &fOBBRad);
		vOBBSentor.y = 0.f;

		fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCellSentor) - XMLoadFloat3(&vOBBSentor)));
		if (fCellRad + fOBBRad > fDis)
			return true;

		// 256
		fOBBRad = 0.f;
		vOBBSentor = Compute_MaxRad(XMLoadFloat3(&vPoints[2]),
			XMLoadFloat3(&vPoints[5]),
			XMLoadFloat3(&vPoints[6]), &fOBBRad);
		vOBBSentor.y = 0.f;

		fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCellSentor) - XMLoadFloat3(&vOBBSentor)));
		if (fCellRad + fOBBRad > fDis)
			return true;

	}


	return false;
}





COBB::OBBDESC COBB::Compute_OBBDesc()
{
	OBBDESC			OBBDesc;

	_float3			vPoints[8];

	m_pOBB->GetCorners(vPoints);

	XMStoreFloat3(&OBBDesc.vCenter, (XMLoadFloat3(&vPoints[2]) + XMLoadFloat3(&vPoints[4])) * 0.5f);

	XMStoreFloat3(&OBBDesc.vCenterAxis[0], (XMLoadFloat3(&vPoints[2]) + XMLoadFloat3(&vPoints[5])) * 0.5f -
		XMLoadFloat3(&OBBDesc.vCenter));
	XMStoreFloat3(&OBBDesc.vCenterAxis[1], (XMLoadFloat3(&vPoints[2]) + XMLoadFloat3(&vPoints[7])) * 0.5f -
		XMLoadFloat3(&OBBDesc.vCenter));
	XMStoreFloat3(&OBBDesc.vCenterAxis[2], (XMLoadFloat3(&vPoints[2]) + XMLoadFloat3(&vPoints[0])) * 0.5f -
		XMLoadFloat3(&OBBDesc.vCenter));

	XMStoreFloat3(&OBBDesc.vAlignAxis[0], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[0])));
	XMStoreFloat3(&OBBDesc.vAlignAxis[1], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[1])));
	XMStoreFloat3(&OBBDesc.vAlignAxis[2], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[2])));

	return OBBDesc;
}

_float COBB::Compute_Height(_fvector vPos, _float3 * vPoss)
{
	_vector vA = XMVectorSetW(XMLoadFloat3(&vPoss[0]), 1.f);
	_vector vB = XMVectorSetW(XMLoadFloat3(&vPoss[1]), 1.f);
	_vector vC = XMVectorSetW(XMLoadFloat3(&vPoss[2]), 1.f);


	_vector vPlane = XMPlaneFromPoints(vA, vB, vC);

	_vector fTargetPos = vPos;
	_float fx = XMVectorGetX(fTargetPos);
	_float fz = XMVectorGetZ(fTargetPos);

	_float fa = XMVectorGetX(vPlane);
	_float fb = XMVectorGetY(vPlane);
	_float fc = XMVectorGetZ(vPlane);
	_float fd = XMVectorGetW(vPlane);

	/* y = (-ax - cz - d) / b */

	return (-fa * fx - fc * fz - fd) / fb;
}

_bool COBB::Compute_LayPlane(_fvector vSentorPos, _fvector vPos, _fvector vDir, _float3 * Out_pPoss, _float* Out_fDot, _float* Out_fY)
{
	_float3			vPoints[8];
	m_pOBB->GetCorners(vPoints); // 나무의 OBB

	_float fMinDis = FLT_MAX;
	_float fDis = FLT_MAX;
	_bool bInter = false;

	_vector vVPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 0.5f);
	_vector vVDir = vDir;

	_float3 vTempPoss[3];

	// 상
	if (TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[3]), XMLoadFloat3(&vPoints[2]), XMLoadFloat3(&vPoints[7]), fDis) ||
		TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[2]), XMLoadFloat3(&vPoints[6]), XMLoadFloat3(&vPoints[7]), fDis))
	{
		if (fMinDis > fDis)
		{
			vTempPoss[0] = vPoints[3];
			vTempPoss[1] = vPoints[2];
			vTempPoss[2] = vPoints[7];
			fMinDis = fDis;
		}

		bInter = true;
	}
	
	// 하
	if (TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[0]), XMLoadFloat3(&vPoints[5]), XMLoadFloat3(&vPoints[1]), fDis) ||
		TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[0]), XMLoadFloat3(&vPoints[5]), XMLoadFloat3(&vPoints[4]), fDis))
	{
		if (fMinDis > fDis)
		{
			vTempPoss[0] = vPoints[0];
			vTempPoss[1] = vPoints[5];
			vTempPoss[2] = vPoints[1];
			fMinDis = fDis;
		}

		bInter = true;
	}

	// 좌
	if (TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[3]), XMLoadFloat3(&vPoints[7]), XMLoadFloat3(&vPoints[0]), fDis) ||
		TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[7]), XMLoadFloat3(&vPoints[4]), XMLoadFloat3(&vPoints[0]), fDis))

	{
		if (fMinDis > fDis)
		{
			vTempPoss[0] = vPoints[3];
			vTempPoss[1] = vPoints[7];
			vTempPoss[2] = vPoints[0];
			fMinDis = fDis;
		}

		bInter = true;
	}

	// 우
	if (TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[6]), XMLoadFloat3(&vPoints[2]), XMLoadFloat3(&vPoints[1]), fDis) ||
		TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[6]), XMLoadFloat3(&vPoints[1]), XMLoadFloat3(&vPoints[5]), fDis))
	{
		if (fMinDis > fDis)
		{
			vTempPoss[0] = vPoints[6];
			vTempPoss[1] = vPoints[2];
			vTempPoss[2] = vPoints[1];
			fMinDis = fDis;
		}

		bInter = true;
	}

	// 앞
	if (TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[7]), XMLoadFloat3(&vPoints[5]), XMLoadFloat3(&vPoints[4]), fDis) ||
		TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[7]), XMLoadFloat3(&vPoints[6]), XMLoadFloat3(&vPoints[5]), fDis))
	{
		if (fMinDis > fDis)
		{
			vTempPoss[0] = vPoints[7];
			vTempPoss[1] = vPoints[5];
			vTempPoss[2] = vPoints[4];
			fMinDis = fDis;
		}

		bInter = true;
	}

	// 뒤
	if (TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[3]), XMLoadFloat3(&vPoints[1]), XMLoadFloat3(&vPoints[2]), fDis) ||
		TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[3]), XMLoadFloat3(&vPoints[0]), XMLoadFloat3(&vPoints[1]), fDis))
	{
		if (fMinDis > fDis)
		{
			vTempPoss[0] = vPoints[3];
			vTempPoss[1] = vPoints[1];
			vTempPoss[2] = vPoints[2];
			fMinDis = fDis;
		}

		bInter = true;
	}




	// 내적 값이 제일 작은 애의 면 정보를 담는다.
	if (bInter)
	{
		_vector vAtoB = XMLoadFloat3(&vTempPoss[1]) - XMLoadFloat3(&vTempPoss[0]);
		_vector vBtoC = XMLoadFloat3(&vTempPoss[2]) - XMLoadFloat3(&vTempPoss[1]);

		_vector vPlanNorDir = XMVector3Cross(vAtoB, vBtoC);
		_vector vMyNorDir = -1.f * vVDir;

		_float fDotTemp = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlanNorDir), XMVector3Normalize(vMyNorDir)));


		// 둔각이라면 평면에 레이를 맞춘게 아니다!
		if (0.f >= fDotTemp)
		{
			bInter = false;
		}
		else
		{
			_float fY = Compute_Height(vSentorPos, vTempPoss);

			if(nullptr != Out_fY && *Out_fY > fY)
			{
				// if (*Out_fDot < fDotTemp)
				Out_pPoss[0] = vTempPoss[0];
				Out_pPoss[1] = vTempPoss[1];
				Out_pPoss[2] = vTempPoss[2];

				*Out_fDot = fDotTemp;
				*Out_fY = fY;
			}
		}

	}

	return bInter;
}

_float3 COBB::Copute_CellY(_float3 vPos, _float fa, _float fb, _float fc, _float fd)
{
	_vector fTargetPos = XMLoadFloat3(&vPos);
	_float fx, fz, fCellY;

	fx = XMVectorGetX(fTargetPos);
	fz = XMVectorGetZ(fTargetPos);

	fCellY = (-fa * fx - fc * fz - fd) / fb;
	fTargetPos = XMVectorSetY(fTargetPos, fCellY);


	_float3 vCellPos;
	XMStoreFloat3(&vCellPos, fTargetPos);

	return vCellPos;
}
_bool COBB::Check_Area(_fvector vA, _fvector vB, _fvector vC, _float fMaxArea)
{
	_float3 vAB; XMStoreFloat3(&vAB, vB - vA);
	_float3 vAC; XMStoreFloat3(&vAC, vC - vA);

	_float fArea = sqrt(pow(((vAB.y*vAC.z) - (vAC.y*vAB.z)), 2) + pow(((vAC.x*vAB.z) - (vAB.x*vAC.z)), 2) + pow(((vAB.x*vAC.y) - (vAC.x*vAB.y)), 2));

	

	if (fMaxArea >= fArea)
		return false;

	return true;
}

_float3 COBB::Compute_MaxRad(_fvector vA, _fvector vB, _fvector vC, _float* Out_MaxRad)
{
	// 셀의 중점에서 가장 먼 점과의 거리를 알아낸다.
	// 셀의 중점
	_fvector vSentor = (vA + vB + vC) / 3.f;
	_float fA = XMVectorGetX(XMVector3Length(vA - vSentor));
	_float fB = XMVectorGetX(XMVector3Length(vB - vSentor));
	_float fC = XMVectorGetX(XMVector3Length(vC - vSentor));

	// 가정 먼 셀과의 거리
	_float fRad = max(fA, fB);
	*Out_MaxRad = max(fRad, fC);
	_float3 vfSentor;
	XMStoreFloat3(&vfSentor, vSentor);
	return vfSentor;
}

void COBB::Compute_State(CNavigation* pNavi, CTransform* pTran)
{

	{
		//{
		//	for (auto& Data : m_eTickWallColState)
		//	{
		//		Set_ColState(Data.eState);
		//		_float fDis = Data.fDis;
		//		_fvector vNorPushDir = XMVector3Normalize(XMLoadFloat3(&Data.vPushDir));
		//		COBB* pStaticOBB = Data.pModel;

		//		switch (m_eColState)
		//		{
		//		case Engine::COBB::COL_NONE:
		//			break;
		//		case Engine::COBB::COL_DOWN:
		//		{
		//			// 밑면과 부딪혔다면 그대로 떨어진다
		//			_vector vPreToCurDir = XMLoadFloat3(&pTran->Get_PrePos()) - pTran->Get_State(CTransform::STATE_POSITION);
		//			_float fPreToCurDis = fabs(XMVectorGetX(XMVector3Length(vPreToCurDir)));

		//			pTran->ResetGravity();
		//			pTran->Set_CurSpeed(0.f);
		//			pTran->Push_Dir(vNorPushDir, fPreToCurDis + 0.1f, pNavi);
		//		}
		//		break;
		//		case Engine::COBB::COL_BLOCK:
		//		{
		//			// 벽과 부딪혔다면 그대로 밀어낸다
		//			_vector vPreToCurDir = XMLoadFloat3(&pTran->Get_PrePos()) - pTran->Get_State(CTransform::STATE_POSITION);
		//			_float fPreToCurDis = fabs(XMVectorGetX(XMVector3Length(vPreToCurDir)));

		//			pTran->ResetGravity();

		//			// 충돌 풀릴때까지 조금씩 밀자..
		//			while (DontTouchMe(pStaticOBB) && 0.01f < (XMVectorGetX(XMVector3Length(vNorPushDir))))
		//			{
		//				pTran->Push_Dir(vNorPushDir, 0.01f, pNavi);
		//				m_pOriginal_OBB->Transform(*m_pOBB, pTran->Get_WorldMatrix());
		//			}
		//		}
		//		break;
		//		case Engine::COBB::COL_SLIDE:
		//		{
		//			pTran->ResetGravity();
		//			_vector vPos = pTran->Get_State(CTransform::STATE_POSITION);
		//			vPos = XMVectorSetY(vPos, fDis);
		//			pTran->Set_State(CTransform::STATE_POSITION, vPos);
		//		}
		//		break;
		//		case Engine::COBB::COL_ON:
		//		{
		//			pTran->ResetGravity();
		//			_vector vPos = pTran->Get_State(CTransform::STATE_POSITION);
		//			vPos = XMVectorSetY(vPos, fDis);
		//			pTran->Set_State(CTransform::STATE_POSITION, vPos);
		//		}
		//		break;
		//		}

		//	}
		//}



		//{
		//	CHECKSTATE MaxData;
		//	ZeroMemory(&MaxData, sizeof(CHECKSTATE));
		//	for (auto& Data : m_eTickColState)
		//	{
		//		if (MaxData.eState < Data.eState)
		//			MaxData = Data;
		//	}


		//	_float fDis = MaxData.fDis;
		//	_fvector vNorPushDir = XMVector3Normalize(XMLoadFloat3(&MaxData.vPushDir));
		//	COBB* pStaticOBB = MaxData.pModel;


		//	Set_ColState(MaxData.eState);

		//	switch (m_eColState)
		//	{
		//	case Engine::COBB::COL_NONE:
		//		break;
		//	case Engine::COBB::COL_DOWN:
		//	{
		//		// 밑면과 부딪혔다면 그대로 떨어진다
		//		_vector vPreToCurDir = XMLoadFloat3(&pTran->Get_PrePos()) - pTran->Get_State(CTransform::STATE_POSITION);
		//		_float fPreToCurDis = fabs(XMVectorGetX(XMVector3Length(vPreToCurDir)));

		//		pTran->ResetGravity();
		//		pTran->Set_CurSpeed(0.f);
		//		pTran->Push_Dir(vNorPushDir, fPreToCurDis + 0.1f, pNavi);
		//	}
		//	break;
		//	case Engine::COBB::COL_BLOCK:
		//	{
		//		// 벽과 부딪혔다면 그대로 밀어낸다
		//		_vector vPreToCurDir = XMLoadFloat3(&pTran->Get_PrePos()) - pTran->Get_State(CTransform::STATE_POSITION);
		//		_float fPreToCurDis = fabs(XMVectorGetX(XMVector3Length(vPreToCurDir)));

		//		pTran->ResetGravity();

		//		// 충돌 풀릴때까지 조금씩 밀자..
		//		while (DontTouchMe(pStaticOBB) && 0.01f < (XMVectorGetX(XMVector3Length(vNorPushDir))))
		//		{
		//			pTran->Push_Dir(vNorPushDir, 0.01f, pNavi);
		//			m_pOriginal_OBB->Transform(*m_pOBB, pTran->Get_WorldMatrix());
		//		}
		//	}
		//	break;
		//	case Engine::COBB::COL_SLIDE:
		//	{
		//		pTran->ResetGravity();
		//		_vector vPos = pTran->Get_State(CTransform::STATE_POSITION);
		//		vPos = XMVectorSetY(vPos, fDis);
		//		pTran->Set_State(CTransform::STATE_POSITION, vPos);
		//	}
		//	break;
		//	case Engine::COBB::COL_ON:
		//	{
		//		pTran->ResetGravity();
		//		_vector vPos = pTran->Get_State(CTransform::STATE_POSITION);
		//		vPos = XMVectorSetY(vPos, fDis);
		//		pTran->Set_State(CTransform::STATE_POSITION, vPos);
		//	}
		//	break;
		//	}
		//}


		//m_eTickColState.clear();
		//m_eTickWallColState.clear();
	}

	

	{

	}


	m_eTickColState.clear();
	m_eTickWallColState.clear();
}



HRESULT COBB::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pBatch->Begin();

	DX::Draw(m_pBatch, *m_pOBB, XMLoadFloat4(&m_vColor));

	m_pBatch->End();

	return S_OK;
}

COBB * COBB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CCollider::TYPE eColliderType)
{
	COBB*			pInstance = new COBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX(TEXT("Failed To Created : COBB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * COBB::Clone(void * pArg)
{
	COBB*			pInstance = new COBB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : COBB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void COBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginal_OBB);
	Safe_Delete(m_pOBB);

}
