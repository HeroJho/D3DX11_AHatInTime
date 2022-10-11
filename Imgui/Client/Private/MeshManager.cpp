#include "stdafx.h"
#include "..\Public\MeshManager.h"

#include "GameInstance.h"
#include "Cell.h"
#include "MapManager.h"
#include "StaticModel.h"
#include "MultiThread.h"

IMPLEMENT_SINGLETON(CMeshManager)

CMeshManager::CMeshManager()
{
}


HRESULT CMeshManager::Init(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	if (nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	return S_OK;
}

void CMeshManager::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(DIK_DELETE))
	{
		m_fDeleteKeyTimeAcc += fTimeDelta;
		if (1.f < m_fDeleteKeyTimeAcc)
		{
			Delete_Cell();
		}
	}
	else
		m_fDeleteKeyTimeAcc = 0.f;

	if (pGameInstance->Key_Down(DIK_DELETE))
	{
		Delete_Cell();
	}


	RELEASE_INSTANCE(CGameInstance);
}




CMultiThread * CMeshManager::Get_MultiThread()
{
	return m_pLoading;
}


HRESULT CMeshManager::Add_Cell(_float fDis, _float3* vPoss)
{
	_float3* pPoss = new _float3[3];
	memcpy(pPoss, vPoss, sizeof(_float3) * 3);
	m_TempCells.emplace(fDis, pPoss);
	return S_OK;
}

HRESULT CMeshManager::Comput_Cell()
{
	if (m_TempCells.empty())
		return S_OK;

	_float3* vPoss = new _float3[3];

	_float fMinDis = FLT_MAX_EXP;
	for (auto& Pair : m_TempCells)
	{
		if (fMinDis > Pair.first)
		{
			fMinDis = Pair.first;
			memcpy(vPoss, Pair.second, sizeof(_float3) * 3);
		}

		Safe_Delete(Pair.second);
	}
	m_TempCells.clear();




	//// 시계 방향으로 정리한다 0 1 2
	//Sort_Cell(vPoss);
	//// 마우스 Lay와 Cell의 법선 벡터를 내적하여 2차 Sort를 한다.
	//Sort_CellByDot(vPoss);

	// 똑같은 셀이 등록되어 있는지 확인한다.
	if (Check_Cell(vPoss))
	{
		Safe_Delete(vPoss);
		return S_OK;
	}


	// 추가한다.
	CCell*			pCell = CCell::Create(m_pDevice, m_pContext, vPoss, m_Cells.size());
	if (nullptr == pCell)
	{
		Safe_Delete(vPoss);
		return E_FAIL;
	}

	m_Cells.push_back(pCell);

	Safe_Delete(vPoss);

	return S_OK;
}

HRESULT CMeshManager::Delete_Cell()
{
	CCell* pMinPickedCell = Find_PickingCell();
	if (nullptr == pMinPickedCell)
		return S_OK;

	_uint iIndex = pMinPickedCell->Get_Index();

	m_Cells[iIndex] = nullptr;

	Safe_Release(pMinPickedCell);

	return S_OK;
}

CCell * CMeshManager::Find_PickingCell()
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	_float3			vTempRayDir, vTempRayPos;
	XMVECTOR		vRayDir, vRayPos;

	vRayDir = XMLoadFloat3(&pGameInstance->Get_MouseDir());
	vRayDir = XMVector3Normalize(vRayDir);
	vRayPos = XMLoadFloat3(&pGameInstance->Get_MousePos());
	vRayPos = XMVectorSetW(vRayPos, 1.f);

	_float fMinDis = FLT_MAX_EXP;
	CCell* pMinCell = nullptr;


	for (_uint i = 0; i < m_Cells.size(); ++i)
	{
		CCell* pCell = m_Cells[i];
		if (nullptr == pCell)
			continue;

		_float3 vPosA = pCell->Get_Point(CCell::POINT_A);
		_float3 vPosB = pCell->Get_Point(CCell::POINT_B);
		_float3 vPosC = pCell->Get_Point(CCell::POINT_C);

		_float		fDist;

		_vector vTemp_1 = XMLoadFloat3(&vPosA);
		vTemp_1 = XMVectorSetW(vTemp_1, 1.f);
		_vector vTemp_2 = XMLoadFloat3(&vPosB);
		vTemp_2 = XMVectorSetW(vTemp_2, 1.f);
		_vector vTemp_3 = XMLoadFloat3(&vPosC);
		vTemp_3 = XMVectorSetW(vTemp_3, 1.f);
		if (true == TriangleTests::Intersects(vRayPos, vRayDir, vTemp_1, vTemp_2, vTemp_3, fDist))
		{
			if (fMinDis > fDist)
			{
				fMinDis = fDist;
				pMinCell = m_Cells[i];
			}
		}
	}

	if (fMinDis != FLT_MAX_EXP)
	{
		RELEASE_INSTANCE(CGameInstance);
		return pMinCell;
	}

	RELEASE_INSTANCE(CGameInstance);
	return nullptr;
}




void CMeshManager::Comput_AllObjNaviMesh()
{
	Clear_Cells();

	map<string, CStaticModel*>* pCreatedModels = CMapManager::Get_Instance()->Get_CreatedModels();

	for (auto& Pair : *pCreatedModels)
	{
		_uint iNumMesh = Pair.second->Get_NumMesh();

		for (_uint i = 0; i < iNumMesh; ++i)
		{
			const VTXMODEL* pNonAnimVertices = Pair.second->Get_Mesh_NonAnimVertices(i);
			const FACEINDICES32* pIndieces = Pair.second->Get_Mesh_Indices(i);
			_uint iNumPrimitives = Pair.second->Get_Mesh_NumPrimitives(i);
			CTransform* pTransform = (CTransform*)Pair.second->Get_ComponentPtr(TEXT("Com_Transform"));


			for (_uint j = 0; j < iNumPrimitives; ++j)
			{

				_int iIndices[] = {
					(_int)pIndieces[j]._0,
					(_int)pIndieces[j]._1,
					(_int)pIndieces[j]._2
				};

				_float	fDist;
				_matrix	WorldMatrix = pTransform->Get_WorldMatrix();



				_vector vTemp_1 = XMLoadFloat3(&pNonAnimVertices[iIndices[0]].vPosition);
				vTemp_1 = XMVectorSetW(vTemp_1, 1.f);
				_vector vTemp_2 = XMLoadFloat3(&pNonAnimVertices[iIndices[1]].vPosition);
				vTemp_2 = XMVectorSetW(vTemp_2, 1.f);
				_vector vTemp_3 = XMLoadFloat3(&pNonAnimVertices[iIndices[2]].vPosition);
				vTemp_3 = XMVectorSetW(vTemp_3, 1.f);

				vTemp_1 = XMVector3TransformCoord(vTemp_1, WorldMatrix);
				vTemp_2 = XMVector3TransformCoord(vTemp_2, WorldMatrix);
				vTemp_3 = XMVector3TransformCoord(vTemp_3, WorldMatrix);

				_vector vAB = vTemp_2 - vTemp_1;
				_vector vBA = vTemp_3 - vTemp_2;
				// 외적한다.
				_vector vNormal = XMVector3Normalize(XMVector3Cross(vAB, vBA));
				vNormal = XMVector3Normalize(vNormal);
				_float fDot = XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 1.f, 0.f, 0.f), vNormal));
				if (m_fCosRatio < fDot)
				{
					_float3 vPoss[3];
					XMStoreFloat3(&vPoss[0], vTemp_1);
					XMStoreFloat3(&vPoss[1], vTemp_2);
					XMStoreFloat3(&vPoss[2], vTemp_3);
					Add_Cell(vPoss);
				}

			}

		}

	}

}

HRESULT CMeshManager::Add_Cell(_float3 * vPoss)
{
	// 시계 방향으로 정리한다 0 1 2
	// Sort_Cell(vPoss);
	//// 마우스 Lay와 Cell의 법선 벡터를 내적하여 2차 Sort를 한다.
	//Sort_CellByDot(vPoss);

	//// 똑같은 셀이 등록되어 있는지 확인한다.
	//if (Check_Cell(vPoss))
	//	return S_OK;

	// 추가한다.
	CCell*			pCell = CCell::Create(m_pDevice, m_pContext, vPoss, m_Cells.size());
	if (nullptr == pCell)
		return E_FAIL;
	m_Cells.push_back(pCell);

	return S_OK;
}

void CMeshManager::Clear_Cells()
{
	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
	m_Cells.clear();
}

HRESULT CMeshManager::Ready_Neighbor()
{
	if (nullptr != m_pLoading)
		return E_FAIL;

	m_pLoading = CMultiThread::Create(m_pDevice, m_pContext, CMultiThread::LOADING_READYNEIGHBOR);
	if (nullptr == m_pLoading)
		return E_FAIL;

	return S_OK;
}

_bool CMeshManager::Check_Cell(_float3 * vPoss)
{
	for (_uint i = 0; i < m_Cells.size(); ++i)
	{
		if (nullptr == m_Cells[i])
			continue;

		_float3 vAPoint = m_Cells[i]->Get_Point(CCell::POINT_A);
		_float3 vBPoint = m_Cells[i]->Get_Point(CCell::POINT_B);
		_float3 vCPoint = m_Cells[i]->Get_Point(CCell::POINT_C);

		if (XMVector3Equal(XMLoadFloat3(&vPoss[0]), XMLoadFloat3(&vAPoint))
			&& XMVector3Equal(XMLoadFloat3(&vPoss[1]), XMLoadFloat3(&vBPoint))
			&& XMVector3Equal(XMLoadFloat3(&vPoss[2]), XMLoadFloat3(&vCPoint)))
		{
			return true;
		}
	}

	return false;
}
void CMeshManager::Sort_Cell(_float3 * vPoss)
{
	_float3 vTempPoss[3];

	memcpy(vTempPoss, vPoss, sizeof(_float3) * 3);

	// 정렬
	for (_uint i = 0; i < 2; ++i)
	{
		_uint iMinIndex = i;
		for (_uint j = i+1; j < 3; ++j)
		{
			if (vTempPoss[iMinIndex].x > vTempPoss[j].x)
			{
				iMinIndex = j;
			}
		}

		_float3 Temp;
		Temp = vTempPoss[i];
		vTempPoss[i] = vTempPoss[iMinIndex];
		vTempPoss[iMinIndex] = Temp;
	}

	// 중간 x값을 찾는다. [1]  
	// 작은 x값을 찾는다. [0]
	// 만약 작은 x값의 z값이 중간 x값의 z값보다 작다면 반대로
	if (vTempPoss[0].z < vTempPoss[1].z)
	{
		vPoss[0] = vTempPoss[1];
		vPoss[1] = vTempPoss[2];
		vPoss[2] = vTempPoss[0];
	}
	else
	{
		vPoss[0] = vTempPoss[1];
		vPoss[1] = vTempPoss[0];
		vPoss[2] = vTempPoss[2];
	}

}
void CMeshManager::Sort_CellByDot(_float3 * vPoss)
{
	_vector vABDir = XMLoadFloat3(&vPoss[1]) - XMLoadFloat3(&vPoss[0]);
	_vector vBCDir = XMLoadFloat3(&vPoss[2]) - XMLoadFloat3(&vPoss[1]);
	_vector vCloss = XMVector3Cross(vABDir, vBCDir);
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_vector vMouseDir = XMLoadFloat3(&pGameInstance->Get_MouseDir()) * -1.f;
	vCloss = XMVector3Normalize(vCloss);
	vMouseDir = XMVector3Normalize(vMouseDir);
	
	if (0.f > XMVectorGetX(XMVector3Dot(vCloss, vMouseDir)))
	{
		_float3 vTemp = vPoss[0];
		vPoss[0] = vPoss[2];
		vPoss[2] = vTemp;
	}

	RELEASE_INSTANCE(CGameInstance);

}







HRESULT CMeshManager::Render()
{
	if (1.f < m_fRendRange)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


		_float4x4			WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

		if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
			return E_FAIL;


		_float3 vCamPos = *(_float3*)&pGameInstance->Get_CamPosition();

		RELEASE_INSTANCE(CGameInstance);
		

		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
			{
				_float fADis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pCell->Get_Point(CCell::POINT_A)) - XMLoadFloat3(&vCamPos)));

				if (m_fRendRange < fADis)
					continue;

				if (FAILED(m_pShader->Set_RawValue("g_vNor", &pCell->Get_Nor(), sizeof(_float3))))
					return E_FAIL;
				m_pShader->Begin(0);
				pCell->Render_Cell();
			}

		}
	}
	

	// =============================================================

	if (nullptr == m_pLoading)
		return S_OK;

	if (m_pLoading->Get_Finished())
	{

		m_pLoading->Get_iNumReadyNeighbor();
		m_pLoading->Get_iNumReadyNeighborMax();



		Safe_Release(m_pLoading);
	}

	return S_OK;
}

void CMeshManager::Free()
{
	Clear_Cells();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pShader);
}
