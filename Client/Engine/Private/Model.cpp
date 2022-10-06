#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Shader.h"


CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext), m_bIsProto(true)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_pBin_AIScene(rhs.m_pBin_AIScene)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_eModelType(rhs.m_eModelType)
	, m_Animations(rhs.m_Animations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_DataMaterials(rhs.m_DataMaterials)
	, m_bIsBin(rhs.m_bIsBin)
	, m_AnimLinearDatas(rhs.m_AnimLinearDatas)
{
	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);


	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);

}

CHierarchyNode * CModel::Get_HierarchyNode(const char * pNodeName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
	{
		return !strcmp(pNodeName, pNode->Get_Name());
	});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return *iter;
}

_uint CModel::Get_MaterialIndex(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Get_MaterialIndex();
}

void CModel::Set_AnimIndex(_uint iAnimIndex)
{
	if (0 > iAnimIndex || m_iNumAnimations < iAnimIndex)
		return;

	if (m_iCurrentAnimIndex == iAnimIndex)
		return;


	// 보간할 애가 있는지 체크
	ANIM_LINEAR_DATA* pLinearData = Get_AnimLinearData(iAnimIndex);
	if (nullptr == pLinearData)
	{
		// 없으면 애니메이션 초기화하고 변화
		m_pCurLinearData = nullptr;
		m_Animations[m_iCurrentAnimIndex]->Init_PlayInfo();
		m_iCurrentAnimIndex = iAnimIndex;
	}
	else
	{
		// 보간 데이터가 존재한다.
		m_pCurLinearData = pLinearData;
	}
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{
	m_bIsBin = false;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	char		szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	_uint		iFlag = 0;

	m_eModelType = eType;

	if (TYPE_NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
	else
		iFlag |= aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;

	m_pAIScene = m_Importer.ReadFile(szFullPath, iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	/*Ready_HierarchyNodes(m_pAIScene->mRootNode, nullptr, 0);

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
	return pSour->Get_Depth() < pDest->Get_Depth();
	});*/

	/* 모델을 구성하는 메시들을 만든다. */
	if (FAILED(Ready_MeshContainers(PivotMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;


	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}


HRESULT CModel::Initialize(void * pArg)
{
	Ready_HierarchyNodes(m_pAIScene->mRootNode, nullptr, 0);

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});


	if (TYPE_ANIM == m_eModelType)
	{
		_uint		iNumMeshes = 0;

		vector<CMeshContainer*>		MeshContainers;

		for (auto& pPrototype : m_Meshes)
		{
			CMeshContainer*		pMeshContainer = (CMeshContainer*)pPrototype->Clone();
			if (nullptr == pMeshContainer)
				return E_FAIL;

			MeshContainers.push_back(pMeshContainer);

			Safe_Release(pPrototype);
		}

		m_Meshes.clear();

		m_Meshes = MeshContainers;

		for (auto& pMeshContainer : m_Meshes)
		{
			if (nullptr != pMeshContainer)
				pMeshContainer->SetUp_HierarchyNodes(this, m_pAIScene->mMeshes[iNumMeshes++]);
		}
	}

	vector<CAnimation*>		Animations;

	for (auto& pPrototype : m_Animations)
	{
		CAnimation*		pAnimation = pPrototype->Clone(this);
		if (nullptr == pAnimation)
			return E_FAIL;

		Animations.push_back(pAnimation);

		Safe_Release(pPrototype);
	}

	m_Animations.clear();

	m_Animations = Animations;

	m_AnimLinearDatas.resize(250);

	return S_OK;
}

HRESULT CModel::Bin_Initialize(void * pArg)
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

	int iBin = 1;
	if (TYPE_ANIM == m_eModelType)
	{
		_uint		iNumMeshes = 0;

		vector<CMeshContainer*>		MeshContainers;

		for (auto& pPrototype : m_Meshes)
		{

			CMeshContainer*		pMeshContainer = (CMeshContainer*)pPrototype->Clone(&iBin);
			if (nullptr == pMeshContainer)
				return E_FAIL;

			MeshContainers.push_back(pMeshContainer);

			Safe_Release(pPrototype);
		}

		m_Meshes.clear();

		m_Meshes = MeshContainers;

		for (auto& pMeshContainer : m_Meshes)
		{
			if (nullptr != pMeshContainer)
				pMeshContainer->Bin_SetUp_HierarchyNodes(this, &m_pBin_AIScene->pHeroMesh[iNumMeshes++]);
		}
	}

	vector<CAnimation*>		Animations;

	for (auto& pPrototype : m_Animations)
	{
		CAnimation*		pAnimation = pPrototype->Bin_Clone(this);
		if (nullptr == pAnimation)
			return E_FAIL;

		Animations.push_back(pAnimation);

		Safe_Release(pPrototype);
	}

	m_Animations.clear();

	m_Animations = Animations;

	m_AnimLinearDatas.resize(250);

	return S_OK;
}


HRESULT CModel::SetUp_OnShader(CShader * pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char * pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->Set_SRV(pShader, pConstantName);
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
	_bool IsEnd = false;
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	/* 현재 재생하고자하는 애니메이션이 제어해야할 뼈들의 지역행렬을 갱신해낸다. */
	if (!m_pCurLinearData)
	{
		if (m_Animations[m_iCurrentAnimIndex]->Play_Animation(fTimeDelta))
		{
			IsEnd = true;
		}
	}
	else
	{
		list<KEYFRAME> NextFirstKeyFrams;
		m_Animations[m_pCurLinearData->iTargetIndex]->Get_FirstKeys(&NextFirstKeyFrams);
		if (m_Animations[m_iCurrentAnimIndex]->Play_Animation(m_pCurLinearData, &NextFirstKeyFrams, fTimeDelta))
		{
			m_iCurrentAnimIndex = m_pCurLinearData->iTargetIndex;
			m_pCurLinearData = nullptr;
		}
	}

	/* 지역행렬을 순차적으로(부모에서 자식으로) 누적하여 m_CombinedTransformation를 만든다.  */
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Set_CombinedTransformation();
	}

	return IsEnd;
}

HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex)
{
	_float4x4		BoneMatrices[256];

	if (TYPE_ANIM == m_eModelType)
	{
		m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

		if (FAILED(pShader->Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 256)))
			return E_FAIL;
	}

	pShader->Begin(0);

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Delete_Anim(_uint iIndex)
{
	if (TYPE_ANIM != m_eModelType)
		return E_FAIL;
	if (0 > iIndex && m_iNumAnimations <= iIndex)
		return E_FAIL;

	vector<CAnimation*>::iterator iter = m_Animations.begin();
	iter += iIndex;
	Safe_Release(*iter);

	m_Animations.erase(iter);

	--m_iNumAnimations;

}

HRESULT CModel::Push_AnimLinearData(ANIM_LINEAR_DATA Data)
{
	vector<ANIM_LINEAR_DATA>* Temps = &m_AnimLinearDatas[Data.iMyIndex];
	for (auto& TData : *Temps)
	{
		if (Data.iTargetIndex == TData.iTargetIndex)
		{
			TData.fTickPerSeconed = Data.fTickPerSeconed;
			TData.fLimitRatio = Data.fLimitRatio;
			return S_OK;
		}
	}

	m_AnimLinearDatas[Data.iMyIndex].push_back(Data);

	return S_OK;
}

ANIM_LINEAR_DATA* CModel::Get_AnimLinearData(int iTargetIndex)
{
	if (m_AnimLinearDatas[m_iCurrentAnimIndex].empty())
		return nullptr;

	for (auto& data : m_AnimLinearDatas[m_iCurrentAnimIndex])
	{
		if (data.iTargetIndex == iTargetIndex)
		{
			return &data;
		}
	}

	return nullptr;
}

void CModel::Reset_AnimLinearData()
{
	for (_int i = 0; i < m_AnimLinearDatas.size(); ++i)
	{
		m_AnimLinearDatas[i].clear();
	}
	m_AnimLinearDatas.clear();

	m_AnimLinearDatas.resize(250);
}




HRESULT CModel::Ready_MeshContainers(_fmatrix PivotMatrix)
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], this, PivotMatrix);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
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

HRESULT CModel::Ready_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth)
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

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_pAIScene->mNumAnimations; ++i)
	{
		aiAnimation*		pAIAnimation = m_pAIScene->mAnimations[i];

		CAnimation*			pAnimation = CAnimation::Create(pAIAnimation);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}





HRESULT CModel::Get_HierarchyNodeData(DATA_HEROSCENE * pHeroScene)
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

HRESULT CModel::Get_MaterialData(DATA_HEROSCENE * pHeroScene)
{
	pHeroScene->iMaterialCount = m_iNumMaterials;
	pHeroScene->pHeroMaterial = new DATA_HEROMATERIAL[m_iNumMaterials];

	for (_int i = 0; i < m_DataMaterials.size(); ++i)
	{
		memcpy(&pHeroScene->pHeroMaterial[i], &m_DataMaterials[i], sizeof(DATA_HEROMATERIAL));
	}

	return S_OK;
}

HRESULT CModel::Get_MeshData(DATA_HEROSCENE * pNodeData)
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

HRESULT CModel::Get_AnimData(DATA_HEROSCENE * pSceneData)
{
	if (0 == m_iNumAnimations)
	{
		pSceneData->pHeroAnim = nullptr;
		pSceneData->iNumAnimations = 0;
		return S_OK;
	}

	pSceneData->iNumAnimations = m_iNumAnimations;
	pSceneData->pHeroAnim = new DATA_HEROANIM[m_iNumAnimations];

	for (_int i = 0; i < m_iNumAnimations; ++i)
	{
		m_Animations[i]->Get_AnimData(&pSceneData->pHeroAnim[i]);
	}

	return S_OK;
}




CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{
	CModel*			pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, pModelFileName, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CModel::Clone(void * pArg)
{
	CModel*			pInstance = new CModel(*this);

	if (!m_pBin_AIScene)
	{
		if (FAILED(pInstance->Initialize(pArg)))
		{
			MSG_BOX(TEXT("Failed To Cloned : CModel"));
			Safe_Release(pInstance);
		}
	}
	else
	{
		if (FAILED(pInstance->Bin_Initialize(pArg)))
		{
			MSG_BOX(TEXT("Failed To Cloned : CModel"));
			Safe_Release(pInstance);
		}
	}


	return pInstance;
}

void CModel::Free()
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

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	m_Importer.FreeScene();

	if (m_pBin_AIScene && m_bIsProto)
	{
		Safe_Release_Scene();
	}
}

_float CModel::Get_CurAin_TickPerSecond()
{
	return Get_Anim_TickPerSecond(m_iCurrentAnimIndex);
}

void CModel::Set_CurAin_TickPerSecond(_float fTickPerSecond)
{
	Set_Anim_TickPerSecond(m_iCurrentAnimIndex, fTickPerSecond);
}

_float CModel::Get_Anim_TickPerSecond(_int iIndex)
{
	return m_Animations[iIndex]->Get_TickPerSecond();
}

void CModel::Set_Anim_TickPerSecond(_int iIndex, _float fTickperSecond)
{
	m_Animations[iIndex]->Set_TickPerSecond(fTickperSecond);
}

char * CModel::Get_CurAnim_Name()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_Name();
}

_bool CModel::Get_CurAnim_Loop()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_Loop();
}

void CModel::Set_CurAnim_Loop(_bool bLoop)
{
	m_Animations[m_iCurrentAnimIndex]->Set_Loop(bLoop);
}





HRESULT CModel::Bin_Initialize_Prototype(DATA_HEROSCENE* pScene, TYPE eType, const char* pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{
	m_bIsBin = true;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
	m_eModelType = eType;

	m_pBin_AIScene = pScene;

	if (nullptr == m_pBin_AIScene)
		return E_FAIL;



	/* 모델을 구성하는 메시들을 만든다. */
	if (FAILED(Bin_Ready_MeshContainers(PivotMatrix)))
		return E_FAIL;

	if (FAILED(Bin_Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Bin_Ready_Animations()))
		return E_FAIL;

	return S_OK;
}


HRESULT CModel::Bin_Ready_MeshContainers(_fmatrix PivotMatrix)
{
	m_iNumMeshes = m_pBin_AIScene->iMeshCount;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Bin_Create(m_pDevice, m_pContext, m_eModelType, &m_pBin_AIScene->pHeroMesh[i], this, PivotMatrix);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Bin_Ready_Materials(const char* pModelFilePath)
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

HRESULT CModel::Bin_Ready_HierarchyNodes()
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

HRESULT CModel::Bin_Ready_Animations()
{
	m_iNumAnimations = m_pBin_AIScene->iNumAnimations;

	for (_uint i = 0; i < m_pBin_AIScene->iNumAnimations; ++i)
	{
		DATA_HEROANIM*		pAIAnimation = &m_pBin_AIScene->pHeroAnim[i];

		CAnimation*			pAnimation = CAnimation::Bin_Create(pAIAnimation);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}


HRESULT CModel::Safe_Release_Scene()
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


CModel * CModel::Bin_Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, DATA_HEROSCENE* pScene, TYPE eType, const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{
	CModel*			pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Bin_Initialize_Prototype(pScene, eType, pModelFilePath, pModelFileName, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
