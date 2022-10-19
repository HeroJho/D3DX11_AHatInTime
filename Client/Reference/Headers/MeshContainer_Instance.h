#pragma once


#include "VIBuffer.h"
#include "Model_Instance.h"


/* 모델을 구성하는 하나의 메시. */
/* 이 메시를 구성하는 정점, 인덱스 버퍼를 보관한다. */

BEGIN(Engine)

class CMeshContainer_Instance final : public CVIBuffer
{
private:
	CMeshContainer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainer_Instance(const CMeshContainer_Instance& rhs);
	virtual ~CMeshContainer_Instance() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, class CModel_Instance* pModel, _fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo);
	virtual HRESULT Bin_Initialize_Prototype(DATA_HEROMETH* pAIMesh, class CModel_Instance* pModel, _fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo);

	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bin_Initialize(void* pArg);

	virtual HRESULT Render() override;

private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = 0;

private:
	HRESULT Ready_Vertices(const aiMesh* pAIMesh, _fmatrix PivotMatrix);

private:
	HRESULT Bin_Ready_Vertices(DATA_HEROMETH* pAIMesh, _fmatrix PivotMatrix);

public:
	static CMeshContainer_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, class CModel_Instance* pModel, _fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo);
	static CMeshContainer_Instance* Bin_Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, DATA_HEROMETH* pAIMesh, class CModel_Instance* pModel, _fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo);

	virtual CComponent* Clone(void* pArg = nullptr) override;

	virtual void Free() override;



	// For. Data
public:
	void Get_MeshData(DATA_HEROMETH* pMesh);

private:
	VTXMODEL*			m_pNonAnimVertices = nullptr;
	FACEINDICES32*		m_pIndices = nullptr;
	_bool				m_bIsProto = false;

	// For. Instance
	_uint					m_iNumInstance = 0;
	_uint					m_iInstanceStride = 0;
	ID3D11Buffer*			m_pVBInstance = nullptr;

};

END