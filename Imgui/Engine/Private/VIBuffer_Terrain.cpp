#include "..\Public\VIBuffer_Terrain.h"
#include "Picking.h"
#include "Transform.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	
{

}


HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar * pHeighitMapFilePath)
{
	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(pHeighitMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER			fh;
	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);

	BITMAPINFOHEADER			ih;
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	_ulong*						pPixel = new _ulong[ih.biWidth * ih.biHeight];
	ReadFile(hFile, pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

	CloseHandle(hFile);


	// 버텍스 단계에서는 폴리곤을 공유하는 점들을 찾아 외적하여 노말 벡터를 구할 수 없다.
#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iStride = sizeof(VTXNORTEX);

	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(j, (pPixel[iIndex] & 0x000000ff) / 10.0f, i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexture = _float2(j / _float(m_iNumVerticesX - 1), i / _float(m_iNumVerticesZ - 1));
		}
	}



	Safe_Delete_Array(pPixel);

#pragma endregion




	// 모든 정점들의 위치가 구해졌고, 폴리곤을 공유한 점들을 쉽게 찾을 수 있기 때문에 인덱스에서 정점 노말 벡터를 구해준다.
#pragma region INDEXBUFFER
	m_iNumPrimitives = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[] = {
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

	// 법선 벡터의 합들을 노말라이즈 한다.
	for (_uint i = 0; i < m_iNumVertices; ++i)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));


#pragma endregion



	// 버텍스 ,인덱스 버퍼를 생성한다.
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
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




_bool CVIBuffer_Terrain::Picking(CTransform* pTransform, _float3* pOut)
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

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
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
			_vector vTemp_1 = XMLoadFloat3(&m_pVerticesPos[iIndices[0]]);
			vTemp_1 = XMVectorSetW(vTemp_1, 1.f);
			_vector vTemp_2 = XMLoadFloat3(&m_pVerticesPos[iIndices[1]]);
			vTemp_2 = XMVectorSetW(vTemp_2, 1.f);
			_vector vTemp_3 = XMLoadFloat3(&m_pVerticesPos[iIndices[2]]);
			vTemp_3 = XMVectorSetW(vTemp_3, 1.f);
			if (true == TriangleTests::Intersects(vRayPos, vRayDir, vTemp_1, vTemp_2, vTemp_3, fDist))
			{
				_vector	vPickPos = vRayPos + vRayDir * fDist;

				XMStoreFloat3(pOut, XMVector3TransformCoord(vPickPos, WorldMatrix));
				
				Safe_Release(pPicking);
				return true;
			}

			/* 왼쪽 하단. */
			vTemp_1 = XMLoadFloat3(&m_pVerticesPos[iIndices[0]]);
			vTemp_1 = XMVectorSetW(vTemp_1, 1.f);
			vTemp_2 = XMLoadFloat3(&m_pVerticesPos[iIndices[2]]);
			vTemp_2 = XMVectorSetW(vTemp_2, 1.f);
			vTemp_3 = XMLoadFloat3(&m_pVerticesPos[iIndices[3]]);
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

_float CVIBuffer_Terrain::Compute_Height(_float3 vTargetPos)
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



CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar * pHeightMap)
{
	CVIBuffer_Terrain*			pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMap)))
	{
		MSG_BOX(TEXT("Failed To Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*			pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

}
