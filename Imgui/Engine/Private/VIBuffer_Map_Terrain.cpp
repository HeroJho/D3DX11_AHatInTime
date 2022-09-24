#include "..\Public\VIBuffer_Map_Terrain.h"
#include "Picking.h"
#include "Transform.h"

CVIBuffer_Map_Terrain::CVIBuffer_Map_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Map_Terrain::CVIBuffer_Map_Terrain(const CVIBuffer_Map_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)

{

}



HRESULT CVIBuffer_Map_Terrain::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CVIBuffer_Map_Terrain::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	TERRAINDESC* pDesc = (TERRAINDESC*)pArg;
	Create_TerrainMxM(pDesc->iNumVerticesX, pDesc->iNumVerticesY);

	return S_OK;
}







HRESULT CVIBuffer_Map_Terrain::Create_TerrainMxM(_uint iNumVerticesX, _uint iNumVerticesZ)
{

#pragma region VERTEXBUFFER

	m_iNumVertexBuffers = 1;
	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;


	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iStride = sizeof(VTXNORTEX);

	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVerticesPosMxM = new _float3[m_iNumVertices];



	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			/* 루프가 하나씩 진행될때마다 1씩증가하는 인덱스를 얻어오기위한.  */
			_uint		iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVerticesPosMxM[iIndex] = _float3(j, 0.0f, i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexture = _float2(j / _float(m_iNumVerticesX - 1), i / _float(m_iNumVerticesZ - 1));
		}
	}



#pragma endregion


#pragma region INDEXBUFFER
	m_iNumPrimitives = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);


	_uint			iNumFaces = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			/* 루프가 하나씩 진행될때마다 1씩증가하는 인덱스를 얻어오기위한.  */
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSourDir, vDestDir, vNormal;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];

			// 두 점에 대한 벡터 2개를 구한다.
			vSourDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);

			// 외적한다.
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			// 외적 결과를 기존 법선 벡터에 더한다.
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);

			++iNumFaces;



			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];

			// 두 점에 대한 벡터 2개를 구한다.
			vSourDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);

			// 외적한다.
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			// 외적 결과를 기존 법선 벡터에 더한다.
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);

			++iNumFaces;

		}
	}


#pragma endregion


	// 버텍스 ,인덱스 버퍼를 생성한다.
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;				// 버퍼를 다이나믹
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU 접근 가능하게
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;


	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);


	return S_OK;
}

_bool CVIBuffer_Map_Terrain::Picking(CTransform* pTransform, _float3* pOut)
{
	CPicking*		pPicking = CPicking::Get_Instance();
	Safe_AddRef(pPicking);



	_matrix		WorldMatrixInv = pTransform->Get_WorldMatrixInverse();


	_float3			vTempRayDir, vTempRayPos;
	XMVECTOR		vRayDir, vRayPos;
	pPicking->Compute_LocalRayInfo(&vTempRayDir, &vTempRayPos, pTransform);

	vRayPos = XMLoadFloat3(&vTempRayPos);
	vRayDir = XMLoadFloat3(&vTempRayDir);

	vRayDir = XMVector3Normalize(vRayDir);



	XMVECTOR vTempCulPos = vRayPos;
	// ZeroMemory(&vTempCulPos, sizeof(XMVECTOR));

	while (!(0.01f > XMVectorGetY(vTempCulPos) && -0.01f < XMVectorGetY(vTempCulPos)))
	{
		vTempCulPos += vRayDir * 0.005f;

		// 0 보다 위에 있고						이동한 값이 나보다 클 때
		if (0.f < XMVectorGetY(vRayPos) && XMVectorGetY(vRayPos) < XMVectorGetY(vTempCulPos))
		{
			Safe_Release(pPicking);
			return false;
		}
		// 0 보다 밑에 있고 이동한 값이 나보다 작을 때
		if (0.f > XMVectorGetY(vRayPos) && XMVectorGetY(vRayPos) > XMVectorGetY(vTempCulPos))
		{
			Safe_Release(pPicking);
			return false;
		}
		// 너무 멀면 끝
		if (150.f < XMVectorGetX(XMVector3Length(vRayPos - vTempCulPos)))
		{
			Safe_Release(pPicking);
			return false;
		}
	}

	_float3 vCulPos;
	XMStoreFloat3(&vCulPos, vTempCulPos);
	m_iStartZ = vCulPos.z - m_iRange;
	m_iStartX = vCulPos.x - m_iRange;
	m_iEndZ = vCulPos.z + m_iRange;
	m_iEndX = vCulPos.x + m_iRange;

	for (_int i = m_iStartZ; i < m_iEndZ; ++i)
	{
		if (i < 0 || i > m_iNumVerticesZ - 1 )
			continue;

		for (_int j = m_iStartX; j < m_iEndX; ++j)
		{
			if (j < 0 || j > m_iNumVerticesX - 1)
				continue;


			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_float		fU, fV, fDist;
			_matrix	WorldMatrix = pTransform->Get_WorldMatrix();


			/* 오른쪽 상단. */
			_vector vTemp_1 = XMLoadFloat3(&m_pVerticesPosMxM[iIndices[0]]);
			vTemp_1 = XMVectorSetW(vTemp_1, 1.f);
			_vector vTemp_2 = XMLoadFloat3(&m_pVerticesPosMxM[iIndices[1]]);
			vTemp_2 = XMVectorSetW(vTemp_2, 1.f);
			_vector vTemp_3 = XMLoadFloat3(&m_pVerticesPosMxM[iIndices[2]]);
			vTemp_3 = XMVectorSetW(vTemp_3, 1.f);
			if (true == TriangleTests::Intersects(vRayPos, vRayDir, vTemp_1, vTemp_2, vTemp_3, fDist))
			{
				_vector	vPickPos = vRayPos + vRayDir * fDist;

				XMStoreFloat3(pOut, XMVector3TransformCoord(vPickPos, WorldMatrix));

				Safe_Release(pPicking);
				return true;
			}

			/* 왼쪽 하단. */
			vTemp_1 = XMLoadFloat3(&m_pVerticesPosMxM[iIndices[0]]);
			vTemp_1 = XMVectorSetW(vTemp_1, 1.f);
			vTemp_2 = XMLoadFloat3(&m_pVerticesPosMxM[iIndices[2]]);
			vTemp_2 = XMVectorSetW(vTemp_2, 1.f);
			vTemp_3 = XMLoadFloat3(&m_pVerticesPosMxM[iIndices[3]]);
			vTemp_3 = XMVectorSetW(vTemp_3, 1.f);
			if (true == TriangleTests::Intersects(vRayPos, vRayDir, vTemp_1, vTemp_2, vTemp_3, fDist))
			{
				_vector	vPickPos = vRayPos + vRayDir * fDist;

				XMStoreFloat3(pOut, XMVector3TransformCoord(vPickPos, WorldMatrix));

				Safe_Release(pPicking);
				return true;
			}

		}
	}


	Safe_Release(pPicking);
	return false;
}

_float CVIBuffer_Map_Terrain::Compute_Height(_float3 vTargetPos)
{
	/* int(vPosition.y / tilesizey) * tilecntx + int(vPsotiion.x / tilesizex) */

	// TODO

	//_uint		iIndex = _int(vTargetPos.z) * m_iNumVerticesX + _int(vTargetPos.x);

	//_uint		iIndices[] = {
	//	iIndex + m_iNumVerticesX, 
	//	iIndex + m_iNumVerticesX + 1,
	//	iIndex + 1, 
	//	iIndex
	//};

	//_float		fWidth = vTargetPos.x - m_pVerticesPos[iIndices[0]].x;
	//_float		fHeight = m_pVerticesPos[iIndices[0]].z - vTargetPos.z;

	//D3DXPLANE			Plane;

	///* 오른쪽 위 삼각형에 있어. */
	//if (fWidth > fHeight)
	//{
	//	D3DXPlaneFromPoints(&Plane, &m_pVerticesPos[iIndices[0]], &m_pVerticesPos[iIndices[1]], &m_pVerticesPos[iIndices[2]]);

	//}
	//else
	//{
	//	D3DXPlaneFromPoints(&Plane, &m_pVerticesPos[iIndices[0]], &m_pVerticesPos[iIndices[2]], &m_pVerticesPos[iIndices[3]]);
	//}

	/* y = (-ax - cz - d) / b */

	// return (-Plane.a * vTargetPos.x - Plane.c * vTargetPos.z - Plane.d) / Plane.b;	
	return 0;
}







void CVIBuffer_Map_Terrain::Make_Tick_Up(_float fHeight, _float fRad, _float fSharp, _float3 vPoint, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);



	//SubResource.pData
	VTXNORTEX* pVertices = (VTXNORTEX*)SubResource.pData;

	for (_int i = m_iStartZ; i < m_iEndZ; ++i)
	{
		if (i < 0 || i > m_iNumVerticesZ - 1)
			continue;

		for (_int j = m_iStartX; j < m_iEndX; ++j)
		{
			if (j < 0 || j > m_iNumVerticesX - 1)
				continue;

			/* 루프가 하나씩 진행될때마다 1씩증가하는 인덱스를 얻어오기위한.  */
			_uint		iIndex = i * m_iNumVerticesX + j;

			_float3 vPos = pVertices[iIndex].vPosition;

			_float fH = fHeight;
			_float fRange = fRad;

			_float3 vTemp1 = vPoint;
			_float3 vTemp2 = pVertices[iIndex].vPosition;
			vTemp1.y = vTemp2.y = 0.f;

			_vector fDis = XMLoadFloat3(&vTemp1) - XMLoadFloat3(&vTemp2);
			_float fLen = XMVectorGetX(XMVector3Length(fDis));

			if (fRange > fLen)
			{
				_float fAcc = (fRange - fLen) / fRange;

				fAcc = pow(fAcc, (1.f / fSharp));

				_float fTempY = fH * fAcc;
				if (fH > 0.01f)
				{
					if (fTempY > vPos.y)
						vPos.y = fTempY;
				}
				else if (fH < -0.01f)
				{
					vPos.y += fTempY * fTimeDelta;
				}
				else
				{
					vPos.y = 0.f;
				}

			}

			pVertices[iIndex].vPosition = vPos;
		}

	}
	m_pContext->Unmap(m_pVB, 0);
}

void CVIBuffer_Map_Terrain::Make_Tick_Down(_float fHeight, _float fRad, _float fSharp, _float3 vPoint)
{


	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	//SubResource.pData
	VTXNORTEX* pVertices = (VTXNORTEX*)SubResource.pData;

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			/* 루프가 하나씩 진행될때마다 1씩증가하는 인덱스를 얻어오기위한.  */
			_uint		iIndex = i * m_iNumVerticesX + j;

			_float3 vPos = pVertices[iIndex].vPosition;

			_float fH = fHeight;
			_float fRange = fRad;

			_float3 vTemp1 = vPoint;
			_float3 vTemp2 = pVertices[iIndex].vPosition;
			vTemp1.y = vTemp2.y = 0.f;

			_vector fDis = XMLoadFloat3(&vTemp1) - XMLoadFloat3(&vTemp2);
			_float fLen = XMVectorGetX(XMVector3Length(fDis));

			if (fRange > fLen)
			{
				_float fAcc = (fRange - fLen) / fRange;

				fAcc = pow(fAcc, (1.f / fSharp));

				_float fTempY = fH * fAcc;

				if(fH < vPos.y)
					vPos.y -= fTempY;
			}

			pVertices[iIndex].vPosition = vPos;
		}

	}
	m_pContext->Unmap(m_pVB, 0);
}






_int CVIBuffer_Map_Terrain::Get_Index(_float3 vPos)
{
	return _int(vPos.x) + _int(vPos.z) * m_iNumVerticesX;
}








CVIBuffer_Map_Terrain * CVIBuffer_Map_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Map_Terrain*			pInstance = new CVIBuffer_Map_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVIBuffer_Map_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CVIBuffer_Map_Terrain::Clone(void * pArg)
{
	CVIBuffer_Map_Terrain*			pInstance = new CVIBuffer_Map_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVIBuffer_Map_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Map_Terrain::Free()
{
	__super::Free();

}
