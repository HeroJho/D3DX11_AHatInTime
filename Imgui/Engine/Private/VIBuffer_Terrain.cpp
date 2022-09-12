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


//HRESULT CVIBuffer_Terrain::Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ)
//{
//	m_iNumVerticesX = iNumVerticesX;
//	m_iNumVerticesZ = iNumVerticesZ;
//
//	m_iStride = sizeof(VTXTEX);
//	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
//	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 /*| D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE3(1)*/;
//	m_ePrimitiveType = D3DPT_TRIANGLELIST;
//	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
//
//	if (FAILED(__super::Create_VertexBuffer()))
//		return E_FAIL;
//
//	VTXTEX*		pVertices = nullptr;
//	
//	m_pVB->Lock(0, 0, (void**)&pVertices, 0);
//
//	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
//	{
//		for (_uint j = 0; j < m_iNumVerticesX; ++j)
//		{
//			/* 루프가 하나씩 진행될때마다 1씩증가하는 인덱스를 얻어오기위한.  */
//			_uint		iIndex = i * m_iNumVerticesX + j;
//
//			pVertices[iIndex].vPosition = _float3(j, 0.0f, i);
//			pVertices[iIndex].vTexture = _float2(j / _float(m_iNumVerticesX - 1) * 30.f, i / _float(m_iNumVerticesZ - 1) * 30.f);
//		}
//	}
//	
//	m_pVB->Unlock();
//
//	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
//	m_eIndexFormat = D3DFMT_INDEX32;
//
//	if (FAILED(__super::Create_IndexBuffer()))
//		return E_FAIL;
//
//	FACEINDICES32*		pIndices = nullptr;
//
//	m_pIB->Lock(0, 0, (void**)&pIndices, 0);
//
//	_uint			iNumFaces = 0;
//
//	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
//	{
//		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
//		{
//			/* 루프가 하나씩 진행될때마다 1씩증가하는 인덱스를 얻어오기위한.  */
//			_uint		iIndex = i * m_iNumVerticesX + j;
//
//			_uint		iIndices[4] = {
//				iIndex + m_iNumVerticesX,
//				iIndex + m_iNumVerticesX + 1,
//				iIndex + 1,
//				iIndex
//			};
//
//			pIndices[iNumFaces]._0 = iIndices[0];
//			pIndices[iNumFaces]._1 = iIndices[1];
//			pIndices[iNumFaces]._2 = iIndices[2];
//			++iNumFaces;
//
//			pIndices[iNumFaces]._0 = iIndices[0];
//			pIndices[iNumFaces]._1 = iIndices[2];
//			pIndices[iNumFaces]._2 = iIndices[3];
//			++iNumFaces;
//		}
//	}
//
//	m_pIB->Unlock();
//	
//	return S_OK;
//}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar * pHeightMap)
{
	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(pHeightMap, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER			fh;
	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);

	BITMAPINFOHEADER			ih;
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	_ulong*						pPixel = new _ulong[ih.biWidth * ih.biHeight];
	ReadFile(hFile, pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

	CloseHandle(hFile);

#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iStride = sizeof(VTXNORTEX);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = _float3(j, (pPixel[iIndex] & 0x000000ff) / 20.0f, i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexture = _float2(j / _float(m_iNumVerticesX - 1), i / _float(m_iNumVerticesZ - 1));
		}
	}
	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pPixel);

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


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

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];
			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];
			++iNumFaces;
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

#pragma endregion


	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void * pArg)
{
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

			// TODO: 피킹
			///* 오른쪽 상단. */
			//if (true == D3DXIntersectTri(&m_pVerticesPos[iIndices[0]], &m_pVerticesPos[iIndices[1]], &m_pVerticesPos[iIndices[2]], &vRayPos, &vRayDir, &fU, &fV, &fDist))
			//{
			//	_float3	vPickPos = vRayPos + vRayDir * fDist;

			//	D3DXVec3TransformCoord(pOut, &vPickPos, &WorldMatrix);

			//	Safe_Release(pPicking);
			//	return true;
			//}

			///* 왼쪽 하단. */
			//if (true == D3DXIntersectTri(&m_pVerticesPos[iIndices[0]], &m_pVerticesPos[iIndices[2]], &m_pVerticesPos[iIndices[3]], &vRayPos, &vRayDir, &fU, &fV, &fDist))
			//{
			//	_float3	vPickPos = vRayPos + vRayDir * fDist;

			//	D3DXVec3TransformCoord(pOut, &vPickPos, &WorldMatrix);

			//	Safe_Release(pPicking);
			//	return true;
			//}
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

//CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumVerticesX, _uint iNumVerticesZ)
//{
//	CVIBuffer_Terrain*			pInstance = new CVIBuffer_Terrain(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype(iNumVerticesX, iNumVerticesZ)))
//	{
//		MSG_BOX(TEXT("Failed To Created : CVIBuffer_Terrain"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

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
