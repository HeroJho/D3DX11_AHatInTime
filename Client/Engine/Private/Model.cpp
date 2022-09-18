#include "..\Public\Model.h"
#include "MeshContainer.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_Meshes(rhs.m_Meshes)
{
	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);


}

HRESULT CModel::Initialize_Prototype(TYPE eType, const char * pModelFilePath, const char * pModelFileName)
{
	char		szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	_uint		iFlag = 0;

	if (TYPE_NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
	else
		iFlag |= aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;

	m_pAIScene = m_Importer.ReadFile(szFullPath, iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	/* 모델을 구성하는 메시들을 만든다. */
	if (FAILED(Ready_MeshContainers()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CModel::Render()
{
	for (auto& pMeshContainer : m_Meshes)
	{
		pMeshContainer->Render();
	}

	return S_OK;
}

HRESULT CModel::Ready_MeshContainers()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i]);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const char * pModelFilePath, const char * pModelFileName)
{
	CModel*			pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, pModelFileName)))
	{
		MSG_BOX(TEXT("Failed To Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*			pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);

	m_Meshes.clear();

	m_Importer.FreeScene();
}
