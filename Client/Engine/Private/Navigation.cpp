#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "PipeLine.h"

#include "OBB.h"
#include "GameObject.h"
#include "Transform.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
	, m_NavigationDesc(rhs.m_NavigationDesc)
	, m_pShader(rhs.m_pShader)
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

	Safe_AddRef(m_pShader);
}

HRESULT CNavigation::Initialize_Prototype(vector<class CCell*>* Cells)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	for (auto& pCell : *Cells)
		m_Cells.push_back(pCell);

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{

	if (nullptr != pArg)
		memcpy(&m_NavigationDesc, pArg, sizeof(NAVIGATIONDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

const vector<class CGameObject*>* CNavigation::Get_CurCellColliders()
{
	return m_Cells[m_NavigationDesc.iCurrentIndex]->Get_Colliders();
}

_bool CNavigation::Get_GroundCell(_float3 * pPoss, CTransform* pTran)
{
	_float fTempY = 0.f;
	if (isGround(pTran->Get_State(CTransform::STATE_POSITION), &fTempY))
	{
		CCell* pCell = m_Cells[m_NavigationDesc.iCurrentIndex];

		pPoss[0] = pCell->Get_Point(CCell::POINT_A);
		pPoss[1] = pCell->Get_Point(CCell::POINT_B);
		pPoss[2] = pCell->Get_Point(CCell::POINT_C);

		return true;
	}
	return false;
}

_float CNavigation::Compute_Height(_fvector vPos)
{
	CCell* pCell = m_Cells[m_NavigationDesc.iCurrentIndex];

	_vector vA = XMVectorSetW(XMLoadFloat3(&pCell->Get_Point(CCell::POINT_A)), 1.f);
	_vector vB = XMVectorSetW(XMLoadFloat3(&pCell->Get_Point(CCell::POINT_B)), 1.f);
	_vector vC = XMVectorSetW(XMLoadFloat3(&pCell->Get_Point(CCell::POINT_C)), 1.f);

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

_bool CNavigation::isMove(_fvector vPosition)
{
	_int	iNeighborIndex = -1;


	/* 현재 쎌 안에서 움직였다. */
	/* 나간방향에 이웃이 있다면. 이웃의 인ㄷ게스를 받아오고.
	이웃이 없다면 안채워온다. */
	if (true == m_Cells[m_NavigationDesc.iCurrentIndex]->isIn(vPosition, &iNeighborIndex))
	{
		return true;
	}


	/* 현재 셀을 나갔다. */
	else
	{
		/* 나간방향에 이웃이 있었다면. */
		if (0 <= iNeighborIndex)
		{
			while (true)
			{
				if (0 > iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex))
				{
					/* 커런트 인덱스를 이웃의 인덱스로 바꿔준다. */

					m_NavigationDesc.iCurrentIndex = iNeighborIndex;
					return true;
				}
			}

		}

		/* 나간방향에 이웃이 없었다면. */
		else
			return false;
	}


	return _bool();
}

_bool CNavigation::isGround(_fvector vPosition, _float* OutfCellY)
{
	_float fCellY = Compute_Height(vPosition);
	_float fY = XMVectorGetY(vPosition);

	*OutfCellY = fCellY;

	if (fCellY >= fY)
		return true;

	return false;
}

void CNavigation::Ready_CellCollision(CGameObject* pGameObject)
{
	if (pGameObject->Get_Colliders().empty())
		return;

	_matrix TransformMatrix = ((CTransform*)pGameObject->Get_ComponentPtr(TEXT("Com_Transform")))->Get_WorldMatrix();
	
	COBB* pObb = (COBB*)pGameObject->Get_Colliders().front();

	for (auto& pCell : m_Cells)
	{
		_vector vA = XMVectorSetW(XMLoadFloat3(&pCell->Get_Point(CCell::POINT_A)), 1.f);
		_vector vB = XMVectorSetW(XMLoadFloat3(&pCell->Get_Point(CCell::POINT_B)), 1.f);
		_vector vC = XMVectorSetW(XMLoadFloat3(&pCell->Get_Point(CCell::POINT_C)), 1.f);


		if (pObb->Collision_Cell(vA, vB, vC, TransformMatrix))
		{
			pCell->Set_Color(_float4(1.f, 0.f, 0.f, 1.f));
			pCell->Add_Collider(pGameObject);
		}
	}
}


#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	//CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);

	//_float4x4			WorldMatrix;
	//XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	//if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	//	return E_FAIL;

	//if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	//	return E_FAIL;

	//if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	//	return E_FAIL;

	////if (FAILED(m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4))))
	////	return E_FAIL;

	//RELEASE_INSTANCE(CPipeLine);


	//// m_pShader->Begin(0);


	//if (-1 == m_NavigationDesc.iCurrentIndex)
	//{
	//	for (auto& pCell : m_Cells)
	//	{
	//		if (nullptr != pCell)
	//		{
	//			if (FAILED(m_pShader->Set_RawValue("g_vColor", &pCell->Get_Color(), sizeof(_float4))))
	//				return E_FAIL;

	//			m_pShader->Begin(0);

	//			pCell->Render_Cell();
	//		}

	//	}
	//}




	//return S_OK;


	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
			pCell->Render_Cell();
	}

	return S_OK;





}

#endif // _DEBUG

HRESULT CNavigation::Ready_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				// 너(Dest)는 내(Sour) AB이웃. 
				pSourCell->Set_NeighborIndex(CCell::LINE_AB, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_BC, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_CA, pDestCell);
			}
		}
	}


	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, vector<class CCell*>* Cells)
{
	CNavigation*			pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Cells)))
	{
		MSG_BOX(TEXT("Failed To Created : CNavigation"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*			pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNavigation"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	Safe_Release(m_pShader);

	m_Cells.clear();
}
