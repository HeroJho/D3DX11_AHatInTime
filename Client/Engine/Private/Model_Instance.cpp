#include "..\Public\Model_Instance.h"
#include "MeshContainer_Instance.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Shader.h"


CModel_Instance::CModel_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext), m_bIsProto(true)
{
}

CModel_Instance::CModel_Instance(const CModel_Instance & rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_pBin_AIScene(rhs.m_pBin_AIScene)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_DataMaterials(rhs.m_DataMaterials)
	, m_bIsBin(rhs.m_bIsBin)
{
	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);


	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}
}

CHierarchyNode * CModel_Instance::Get_HierarchyNode(const char * pNodeName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
	{
		return !strcmp(pNodeName, pNode->Get_Name());
	});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return *iter;
}

_uint CModel_Instance::Get_MaterialIndex(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Get_MaterialIndex();
}


HRESULT CModel_Instance::Initialize_Prototype(const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo)
{
	m_bIsBin = false;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	char		szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	_uint		iFlag = 0;


	iFlag |= aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;


	m_pAIScene = m_Importer.ReadFile(szFullPath, iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;


	/* 모델을 구성하는 메시들을 만든다. */
	if (FAILED(Ready_MeshContainers(PivotMatrix, iNumInstance, pInstanceInfo)))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	return S_OK;
}


HRESULT CModel_Instance::Initialize(void * pArg)
{
	Ready_HierarchyNodes(m_pAIScene->mRootNode, nullptr, 0);

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	return S_OK;
}

HRESULT CModel_Instance::Bin_Initialize(void * pArg)
{
	Bin_Ready_HierarchyNodes();

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	for (int i = 0; i < m_pBin_AIScene->iNodeCount; ++i)
	{
		for (int j = 0; j < m_pBin_AIScene->iNodeCount; ++j)
		{
			if (nullptr != m_HierarchyNodes[i]->Get_Parent())
				break;

			m_HierarchyNodes[i]->Set_FindParent(m_HierarchyNodes[j]);
		}
	}

	return S_OK;
}


HRESULT CModel_Instance::SetUp_OnShader(CShader * pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char * pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->Set_SRV(pShader, pConstantName);
}

HRESULT CModel_Instance::Render(CShader* pShader, _uint iMeshIndex, _uint iPassIndex)
{

	pShader->Begin(iPassIndex);

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}




HRESULT CModel_Instance::Ready_MeshContainers(_fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo)
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMeshContainer_Instance*		pMeshContainer = CMeshContainer_Instance::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i], this, PivotMatrix, iNumInstance, pInstanceInfo);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel_Instance::Ready_Materials(const char* pModelFilePath)
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));

		// *
		DATA_HEROMATERIAL		DataMaterialDesc;
		ZeroMemory(&DataMaterialDesc, sizeof(DATA_HEROMATERIAL));


		aiMaterial*			pAIMaterial = m_pAIScene->mMaterials[i];

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString		strPath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;

			char			szFullPath[MAX_PATH] = "";
			char			szFileName[MAX_PATH] = "";
			char			szExt[MAX_PATH] = "";

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			// *
			memcpy(&DataMaterialDesc.cNames[j], &szFileName, sizeof(char) * MAX_PATH);

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar			szWideFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szWideFullPath, MAX_PATH);


			MaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szWideFullPath);
			if (nullptr == MaterialDesc.pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(MaterialDesc);

		// *
		m_DataMaterials.push_back(DataMaterialDesc);
	}

	return S_OK;
}

HRESULT CModel_Instance::Ready_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth)
{
	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode, pParent, iDepth++);

	if (nullptr == pHierarchyNode)
		return E_FAIL;

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		Ready_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth);
	}

	return S_OK;
}



HRESULT CModel_Instance::Get_HierarchyNodeData(DATA_HEROSCENE * pHeroScene)
{
	if (0 == m_HierarchyNodes.size())
	{
		pHeroScene->pHeroNodes = nullptr;
		return S_OK;
	}

	pHeroScene->pHeroNodes = new DATA_HERONODE[m_HierarchyNodes.size()];
	pHeroScene->iNodeCount = m_HierarchyNodes.size();

	for (_int i = 0; i < m_HierarchyNodes.size(); ++i)
	{
		DATA_HERONODE Hero_Node;
		ZeroMemory(&Hero_Node, sizeof(DATA_HERONODE));

		const char* pMyName = m_HierarchyNodes[i]->Get_Name();
		const char* pParentName = m_HierarchyNodes[i]->Get_ParentName();
		memcpy(&Hero_Node.cName, pMyName, sizeof(char) * MAX_PATH);
		memcpy(&Hero_Node.cParent, pParentName, sizeof(char) * MAX_PATH);

		Hero_Node.iDepth = m_HierarchyNodes[i]->Get_Depth();

		Hero_Node.mTransform = m_HierarchyNodes[i]->Get_OriTransformation();

		pHeroScene->pHeroNodes[i] = Hero_Node;
	}

	return S_OK;
}

HRESULT CModel_Instance::Get_MaterialData(DATA_HEROSCENE * pHeroScene)
{
	pHeroScene->iMaterialCount = m_iNumMaterials;
	pHeroScene->pHeroMaterial = new DATA_HEROMATERIAL[m_iNumMaterials];

	for (_int i = 0; i < m_DataMaterials.size(); ++i)
	{
		memcpy(&pHeroScene->pHeroMaterial[i], &m_DataMaterials[i], sizeof(DATA_HEROMATERIAL));
	}

	return S_OK;
}

HRESULT CModel_Instance::Get_MeshData(DATA_HEROSCENE * pNodeData)
{
	pNodeData->iMeshCount = m_iNumMeshes;
	pNodeData->pHeroMesh = new DATA_HEROMETH[m_iNumMeshes];
	for (_int i = 0; i < m_iNumMeshes; ++i)
	{
		DATA_HEROMETH HeroMesh;
		m_Meshes[i]->Get_MeshData(&HeroMesh);
		memcpy(&pNodeData->pHeroMesh[i], &HeroMesh, sizeof(DATA_HEROMETH));
	}

	return S_OK;
}

HRESULT CModel_Instance::Get_AnimData(DATA_HEROSCENE * pSceneData)
{

	pSceneData->pHeroAnim = nullptr;
	pSceneData->iNumAnimations = 0;
	return S_OK;

}




CModel_Instance * CModel_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char * pModelFilePath, const char * pModelFileName, _uint iNumInstance, VTXINSTANCE* pInstanceInfo, _fmatrix PivotMatrix)
{
	CModel_Instance*			pInstance = new CModel_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, pModelFileName, PivotMatrix, iNumInstance, pInstanceInfo)))
	{
		MSG_BOX(TEXT("Failed To Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CModel_Instance::Clone(void * pArg)
{
	CModel_Instance*			pInstance = new CModel_Instance(*this);

	if (!m_pBin_AIScene)
	{
		if (FAILED(pInstance->Initialize(pArg)))
		{
			MSG_BOX(TEXT("Failed To Cloned : CModel_Instance"));
			Safe_Release(pInstance);
		}
	}
	else
	{
		if (FAILED(pInstance->Bin_Initialize(pArg)))
		{
			MSG_BOX(TEXT("Failed To Cloned : CModel_Instance"));
			Safe_Release(pInstance);
		}
	}


	return pInstance;
}

void CModel_Instance::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_Release(pHierarchyNode);

	m_HierarchyNodes.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);

	m_Meshes.clear();

	m_Importer.FreeScene();

	if (m_pBin_AIScene && m_bIsProto)
	{
		Safe_Release_Scene();
	}
}





HRESULT CModel_Instance::Bin_Initialize_Prototype(DATA_HEROSCENE* pScene,  const char* pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo)
{
	m_bIsBin = true;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	m_pBin_AIScene = pScene;

	if (nullptr == m_pBin_AIScene)
		return E_FAIL;



	/* 모델을 구성하는 메시들을 만든다. */
	if (FAILED(Bin_Ready_MeshContainers(PivotMatrix, iNumInstance, pInstanceInfo)))
		return E_FAIL;


	if (FAILED(Bin_Ready_Materials(pModelFilePath)))
		return E_FAIL;


	return S_OK;
}


HRESULT CModel_Instance::Bin_Ready_MeshContainers(_fmatrix PivotMatrix, _uint iNumInstance, VTXINSTANCE* pInstanceInfo)
{
	m_iNumMeshes = m_pBin_AIScene->iMeshCount;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMeshContainer_Instance*		pMeshContainer = CMeshContainer_Instance::Bin_Create(m_pDevice, m_pContext, &m_pBin_AIScene->pHeroMesh[i], this, PivotMatrix, iNumInstance, pInstanceInfo);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel_Instance::Bin_Ready_Materials(const char* pModelFilePath)
{
	if (nullptr == m_pBin_AIScene)
		return E_FAIL;

	m_iNumMaterials = m_pBin_AIScene->iMaterialCount;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));

		DATA_HEROMATERIAL			pAIMaterial = m_pBin_AIScene->pHeroMaterial[i];

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			if (!strcmp(pAIMaterial.cNames[j], ""))
				continue;


			char			szFullPath[MAX_PATH] = "";
			char			szExt[MAX_PATH] = ".dds";

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, pAIMaterial.cNames[j]);
			strcat_s(szFullPath, szExt);

			_tchar			szWideFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szWideFullPath, MAX_PATH);


			MaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szWideFullPath);
			if (nullptr == MaterialDesc.pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(MaterialDesc);
	}

	return S_OK;
}

HRESULT CModel_Instance::Bin_Ready_HierarchyNodes()
{
	for (int i = 0; i < m_pBin_AIScene->iNodeCount; ++i)
	{
		DATA_HERONODE Node = m_pBin_AIScene->pHeroNodes[i];

		CHierarchyNode*		pHierarchyNode = CHierarchyNode::Bin_Create(&Node);
		if (nullptr == pHierarchyNode)
			return E_FAIL;

		m_HierarchyNodes.push_back(pHierarchyNode);
	}

	return S_OK;
}



HRESULT CModel_Instance::Safe_Release_Scene()
{
	Safe_Delete_Array(m_pBin_AIScene->pHeroNodes);
	Safe_Delete_Array(m_pBin_AIScene->pHeroMaterial);

	for (_int i = 0; i < m_pBin_AIScene->iMeshCount; ++i)
	{
		Safe_Delete_Array(m_pBin_AIScene->pHeroMesh[i].pAnimVertices);
		Safe_Delete_Array(m_pBin_AIScene->pHeroMesh[i].pNonAnimVertices);
		Safe_Delete_Array(m_pBin_AIScene->pHeroMesh[i].pIndices);
		Safe_Delete_Array(m_pBin_AIScene->pHeroMesh[i].pBones);
	}
	Safe_Delete_Array(m_pBin_AIScene->pHeroMesh);


	for (_int i = 0; i < m_pBin_AIScene->iNumAnimations; ++i)
	{
		for (_int j = 0; j < m_pBin_AIScene->pHeroAnim[i].iNumChannels; ++j)
		{
			Safe_Delete_Array(m_pBin_AIScene->pHeroAnim[i].pHeroChannel[j].pKeyFrames);
		}
		Safe_Delete_Array(m_pBin_AIScene->pHeroAnim[i].pHeroChannel);
	}
	Safe_Delete_Array(m_pBin_AIScene->pHeroAnim);

	Safe_Delete(m_pBin_AIScene);

	return S_OK;
}


CModel_Instance * CModel_Instance::Bin_Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, DATA_HEROSCENE* pScene,  const char * pModelFilePath, const char * pModelFileName, _uint iNumInstance, VTXINSTANCE* pInstanceInfo, _fmatrix PivotMatrix)
{
	CModel_Instance*			pInstance = new CModel_Instance(pDevice, pContext);

	if (FAILED(pInstance->Bin_Initialize_Prototype(pScene, pModelFilePath, pModelFileName, PivotMatrix, iNumInstance, pInstanceInfo)))
	{
		MSG_BOX(TEXT("Failed To Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
