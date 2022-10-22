#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel_Instance final : public CComponent
{
public:
	typedef struct tagStaticModelDesc
	{
		TCHAR		cModelTag[MAX_PATH];
		_uint		iNumInstance;
		VTXINSTANCE* pLocalInfos;

	}STATICMODELDESC;

private:
	CModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_Instance(const CModel_Instance& rhs);
	virtual ~CModel_Instance() = default;

public:
	class CHierarchyNode* Get_HierarchyNode(const char* pNodeName);

	_uint Get_NumMeshes() const { return m_iNumMeshes; }

	_matrix Get_PivotMatrix() { return XMLoadFloat4x4(&m_PivotMatrix); }

	_uint Get_MaterialIndex(_uint iMeshIndex);



public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo);
	virtual HRESULT Bin_Initialize_Prototype(DATA_HEROSCENE* pScene, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo);

	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bin_Initialize(void* pArg);

public:
	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pConstantName);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);



private:
	const aiScene*				m_pAIScene = nullptr;

	DATA_HEROSCENE*				m_pBin_AIScene = nullptr;
	_bool						m_bIsProto = false;
	_bool						m_bIsBin = false;

	Assimp::Importer			m_Importer;

	_float4x4					m_PivotMatrix;


private:
	_uint									m_iNumMeshes = 0;
	vector<class CMeshContainer_Instance*>			m_Meshes;
	typedef vector<class CMeshContainer_Instance*>	MESHES;

private:
	_uint									m_iNumMaterials = 0;
	vector<MATERIALDESC>					m_Materials;

private:
	vector<class CHierarchyNode*>			m_HierarchyNodes;


private:
	HRESULT Ready_MeshContainers(_fmatrix PivotMatrix, _uint iNumInstance ,VTXINSTANCE* pInstanceInfo);
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_HierarchyNodes(aiNode* pNode, class CHierarchyNode* pParent, _uint iDepth);

private:
	HRESULT Bin_Ready_MeshContainers(_fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo);
	HRESULT Bin_Ready_Materials(const char* pModelFilePath);
	HRESULT Bin_Ready_HierarchyNodes();
	HRESULT Safe_Release_Scene();
public:
	_bool	Get_IsBin() { return m_bIsBin; }




public:
	static CModel_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,  const char* pModelFilePath, const char* pModelFileName, _uint iNumInstance, VTXINSTANCE* pInstanceInfo, _fmatrix PivotMatrix = XMMatrixIdentity());
	static CModel_Instance* Bin_Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, DATA_HEROSCENE* pScene, const char* pModelFilePath, const char* pModelFileName, _uint iNumInstance, VTXINSTANCE* pInstanceInfo, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;




public: // For. Data
	HRESULT Get_HierarchyNodeData(DATA_HEROSCENE* pNodeData);
	HRESULT Get_MaterialData(DATA_HEROSCENE* pNodeData);
	HRESULT Get_MeshData(DATA_HEROSCENE* pNodeData);
	HRESULT Get_AnimData(DATA_HEROSCENE* pAnimData);

private:
	vector<DATA_HEROMATERIAL>				m_DataMaterials;


};

END