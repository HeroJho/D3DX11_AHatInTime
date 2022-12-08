#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	class CHierarchyNode* Get_HierarchyNode(const char* pNodeName);

	_uint Get_NumMeshes() const { return m_iNumMeshes; }

	_matrix Get_PivotMatrix() { return XMLoadFloat4x4(&m_PivotMatrix); }

	_uint Get_MaterialIndex(_uint iMeshIndex);

	void Set_AnimIndex(_uint iAnimIndex, _bool bContinue = false);
	_int Get_AnimIndex() { return m_iNumAnimations; }
	_int Get_CurAnimIndex() { return m_iCurrentAnimIndex; }

	_float Get_CurAin_TickPerSecond();
	void Set_CurAin_TickPerSecond(_float fTickPerSecond);

	_float Get_Anim_TickPerSecond(_int iIndex);
	void Set_Anim_TickPerSecond(_int iIndex, _float fTickperSecond);
	char* Get_CurAnim_Name();

	_bool Get_CurAnim_Loop();
	void Set_CurAnim_Loop(_bool bLoop);

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix);
	virtual HRESULT Bin_Initialize_Prototype(DATA_HEROSCENE* pScene, TYPE eType, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix);

	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bin_Initialize(void* pArg);

public:
	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pConstantName);
	_bool Play_Animation(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);

	HRESULT Delete_Anim(_uint iIndex);
	HRESULT Push_AnimLinearData(ANIM_LINEAR_DATA Data);
	ANIM_LINEAR_DATA* Get_AnimLinearData(int iIndex);
	void Reset_AnimLinearData();


private:
	const aiScene*				m_pAIScene = nullptr;

	DATA_HEROSCENE*				m_pBin_AIScene = nullptr;
	_bool						m_bIsProto = false;
	_bool						m_bIsBin = false;

	Assimp::Importer			m_Importer;

	_float4x4					m_PivotMatrix;
	TYPE						m_eModelType = TYPE_END;

private:
	_uint									m_iNumMeshes = 0;
	vector<class CMeshContainer*>			m_Meshes;
	typedef vector<class CMeshContainer*>	MESHES;

private:
	_uint									m_iNumMaterials = 0;
	vector<MATERIALDESC>					m_Materials;

private:
	vector<class CHierarchyNode*>			m_HierarchyNodes;

private:
	_uint								m_iCurrentAnimIndex = 0;
	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;

	vector<vector<ANIM_LINEAR_DATA>>	m_AnimLinearDatas;
	ANIM_LINEAR_DATA*					m_pCurLinearData = nullptr;

	list<KEYFRAME> m_NextFirstKeyFrams;

private:
	HRESULT Ready_MeshContainers(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_HierarchyNodes(aiNode* pNode, class CHierarchyNode* pParent, _uint iDepth);
	HRESULT Ready_Animations();

private:
	HRESULT Bin_Ready_MeshContainers(_fmatrix PivotMatrix);
	HRESULT Bin_Ready_Materials(const char* pModelFilePath);
	HRESULT Bin_Ready_HierarchyNodes();
	HRESULT Bin_Ready_Animations();
	HRESULT Safe_Release_Scene();




public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix = XMMatrixIdentity());
	static CModel* Bin_Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, DATA_HEROSCENE* pScene, TYPE eType, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix = XMMatrixIdentity());
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