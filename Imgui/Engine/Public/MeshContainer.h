#pragma once


#include "VIBuffer.h"
#include "Model.h"


/* 모델을 구성하는 하나의 메시. */
/* 이 메시를 구성하는 정점, 인덱스 버퍼를 보관한다. */

BEGIN(Engine)

class CMeshContainer final : public CVIBuffer
{
private:
	CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	const VTXMODEL* Get_NonAnimVertices() { return m_pNonAnimVertices; }
	const FACEINDICES32* Get_Indices() { return m_pIndices; }
	_uint Get_NumPrimitives() { return m_iNumPrimitives; }

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual HRESULT Bin_Initialize_Prototype(CModel::TYPE eModelType, DATA_HEROMETH* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);

	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bin_Initialize(void* pArg);

	virtual _bool Picking(class CTransform* pTransform, _float* pOut_Dis, _float3* pOut_Poss);

public:
	HRESULT SetUp_HierarchyNodes(class CModel* pModel, aiMesh* pAIMesh);
	HRESULT Bin_SetUp_HierarchyNodes(class CModel* pModel, DATA_HEROMETH* pAIMesh);
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);


private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = 0;

private:
	/* 이 메시에 영향ㅇ르 주는 뼈의 갯수. */
	_uint							m_iNumBones = 0;

	/* 이 메시에 영향을 주는 뼈들을 모아놓느다. */
	/* why ? : 메시 별로 렌더링할 때 이 메시에 영향ㅇ르 주는 뼏르의 행렬을 모아서 셰{ㅇ디러ㅗ 도ㅓㄴ질려고. */
	vector<class CHierarchyNode*>	m_Bones;

private:
	HRESULT Ready_Vertices(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_AnimVertices(const aiMesh* pAIMesh, CModel* pModel);

private:
	HRESULT Bin_Ready_Vertices(DATA_HEROMETH* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Bin_Ready_AnimVertices(DATA_HEROMETH* pAIMesh, CModel* pModel);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	static CMeshContainer* Bin_Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, DATA_HEROMETH* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);

	virtual CComponent* Clone(void* pArg = nullptr) override;

	virtual void Free() override;



	// For. Data
public:
	void Get_MeshData(DATA_HEROMETH* pMesh);

private:
	VTXMODEL*			m_pNonAnimVertices = nullptr;
	VTXANIMMODEL*		m_pAnimVertices = nullptr;
	FACEINDICES32*		m_pIndices = nullptr;
	_bool				m_bIsProto = false;

};

END