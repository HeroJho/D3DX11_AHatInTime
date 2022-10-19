#include "..\Public\MeshContainer_Instance.h"
#include "Model.h"
#include "HierarchyNode.h"

CMeshContainer_Instance::CMeshContainer_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
	, m_bIsProto(true)
{
}

CMeshContainer_Instance::CMeshContainer_Instance(const CMeshContainer_Instance & rhs)
	: CVIBuffer(rhs)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_pNonAnimVertices(rhs.m_pNonAnimVertices)
	, m_pIndices(rhs.m_pIndices)
	, m_bIsProto(false)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
{

	strcpy_s(m_szName, rhs.m_szName);
	Safe_AddRef(m_pVBInstance);
}





HRESULT CMeshContainer_Instance::Initialize_Prototype(const aiMesh * pAIMesh, CModel_Instance* pModel, _fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo)
{
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumInstance = iNumInstance;

#pragma region VERTEXBUFFER

	HRESULT		hr = 0;

	hr = Ready_Vertices(pAIMesh, PivotMatrix);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = pAIMesh->mNumFaces * iNumInstance;
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

	
	// Instance 갯수만큼 인덱스 버퍼를 중복해서 찍는다.
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		for (_uint j = 0; j < pAIMesh->mNumFaces; ++j)
		{
			_uint iIndex = j + i * pAIMesh->mNumFaces;
			pIndices[iIndex]._0 = pAIMesh->mFaces[j].mIndices[0];
			pIndices[iIndex]._1 = pAIMesh->mFaces[j].mIndices[1];
			pIndices[iIndex]._2 = pAIMesh->mFaces[j].mIndices[2];
		}
	}


	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	m_pIndices = pIndices;

#pragma endregion

#pragma region INSTANCEBUFFER
	m_iInstanceStride = sizeof(VTXINSTANCE);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iInstanceStride * iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	VTXINSTANCE*		pInstanceVtx = new VTXINSTANCE[iNumInstance];
	ZeroMemory(pInstanceVtx, sizeof(VTXINSTANCE) * iNumInstance);

	for (_uint i = 0; i < iNumInstance; ++i)
	{
		memcpy(&pInstanceVtx[0], &pInstanceInfo[0], sizeof(VTXINSTANCE));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pInstanceVtx;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVtx);
	Safe_Delete_Array(pInstanceInfo);


#pragma endregion


	return S_OK;
}
HRESULT CMeshContainer_Instance::Bin_Initialize_Prototype(DATA_HEROMETH* pAIMesh, CModel_Instance* pModel, _fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo)
{
	strcpy_s(m_szName, pAIMesh->cName);
	m_iMaterialIndex = pAIMesh->iMaterialIndex;
	m_iNumInstance = iNumInstance;

#pragma region VERTEXBUFFER

	HRESULT		hr = 0;


	hr = Bin_Ready_Vertices(pAIMesh, PivotMatrix);


	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = pAIMesh->iNumPrimitives * iNumInstance;
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



	// Instance 갯수만큼 인덱스 버퍼를 중복해서 찍는다.
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		for (_uint j = 0; j < pAIMesh->iNumPrimitives; ++j)
		{
			_uint iIndex = j + i * pAIMesh->iNumPrimitives;
			pIndices[iIndex]._0 = pAIMesh->pIndices[j]._0;
 			pIndices[iIndex]._1 = pAIMesh->pIndices[j]._1;
			pIndices[iIndex]._2 = pAIMesh->pIndices[j]._2;
		}
	}


	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCEBUFFER
	m_iInstanceStride = sizeof(VTXINSTANCE);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iInstanceStride * iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	VTXINSTANCE*		pInstanceVtx = new VTXINSTANCE[iNumInstance];
	ZeroMemory(pInstanceVtx, sizeof(VTXINSTANCE) * iNumInstance);

	for (_uint i = 0; i < iNumInstance; ++i)
	{
		memcpy(&pInstanceVtx[0], &pInstanceInfo[0], sizeof(VTXINSTANCE));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pInstanceVtx;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVtx);
	Safe_Delete_Array(pInstanceInfo);


#pragma endregion


	return S_OK;
}

HRESULT CMeshContainer_Instance::Initialize(void * pArg)
{
	return S_OK;
}
HRESULT CMeshContainer_Instance::Bin_Initialize(void * pArg)
{
	return S_OK;
}




HRESULT CMeshContainer_Instance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint			iStrides[] = {
		m_iStride,
		m_iInstanceStride
	};

	_uint			iOffsets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexedInstanced(m_iNumPrimitives * 3, m_iNumInstance, 0, 0, 0);
	
	return S_OK;
}




HRESULT CMeshContainer_Instance::Ready_Vertices(const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMODEL*		pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		memcpy(&pVertices[i].vTexture, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_pNonAnimVertices = pVertices;

	return S_OK;
}
HRESULT CMeshContainer_Instance::Bin_Ready_Vertices(DATA_HEROMETH* pAIMesh, _fmatrix PivotMatrix)
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = pAIMesh->NumVertices;
	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMODEL*		pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i] = pAIMesh->pNonAnimVertices[i];
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}




CMeshContainer_Instance * CMeshContainer_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const aiMesh * pAIMesh, CModel_Instance* pModel, _fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo)
{
	CMeshContainer_Instance*			pInstance = new CMeshContainer_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pAIMesh, pModel, PivotMatrix, iNumInstance, pInstanceInfo)))
	{
		MSG_BOX(TEXT("Failed To Created : CMeshContainer_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
CMeshContainer_Instance * CMeshContainer_Instance::Bin_Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, DATA_HEROMETH* pAIMesh, CModel_Instance* pModel, _fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo)
{
	CMeshContainer_Instance*			pInstance = new CMeshContainer_Instance(pDevice, pContext);

	if (FAILED(pInstance->Bin_Initialize_Prototype(pAIMesh, pModel, PivotMatrix, iNumInstance, pInstanceInfo)))
	{
		MSG_BOX(TEXT("Failed To Created : CBinMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}





CComponent * CMeshContainer_Instance::Clone(void * pArg)
{
	CMeshContainer_Instance*			pInstance = new CMeshContainer_Instance(*this);

	if (!pArg)
	{
		if (FAILED(pInstance->Bin_Initialize(pArg)))
		{
			MSG_BOX(TEXT("Failed To Cloned : CMeshContainer_Instance"));
			Safe_Release(pInstance);
		}
	}
	else
	{
		if (FAILED(pInstance->Initialize(pArg)))
		{
			MSG_BOX(TEXT("Failed To Cloned : CMeshContainer_Instance"));
			Safe_Release(pInstance);
		}
	}


	return pInstance;
}

void CMeshContainer_Instance::Free()
{
	__super::Free();

	if (m_bIsProto)
	{
		Safe_Delete_Array(m_pIndices);
		Safe_Delete_Array(m_pNonAnimVertices);
	}

	Safe_Release(m_pVBInstance);
}




void CMeshContainer_Instance::Get_MeshData(DATA_HEROMETH * pMesh)
{
	memcpy(&pMesh->cName, &m_szName, sizeof(char) * MAX_PATH);
	pMesh->iMaterialIndex = m_iMaterialIndex;
	pMesh->NumVertices = m_iNumVertices;
	pMesh->pNonAnimVertices = m_pNonAnimVertices;
	pMesh->pAnimVertices = nullptr;
	pMesh->iNumPrimitives = m_iNumPrimitives;
	pMesh->pIndices = m_pIndices;

	pMesh->iNumBones = 0;
	pMesh->pBones = new DATA_HEROBONE[0];


}
