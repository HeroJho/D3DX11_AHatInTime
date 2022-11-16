#include "..\Public\Sockat.h"

#include "GameObject.h"
#include "Renderer.h"
#include "HierarchyNode.h"
#include "Transform.h"
#include "GameInstance.h"

CSockat::CSockat(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CSockat::CSockat(const CSockat & rhs)
	: CComponent(rhs)
{

}


string CSockat::Get_SlotTag(_uint eID)
{
	return m_PartsName[eID];
}

CGameObject* CSockat::Get_SlotPos(_uint eID)
{
	return m_Parts[eID];
}

HRESULT CSockat::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CSockat::Initialize(void * pArg)
{
	if (nullptr != pArg)
	{
		SOCATDESC* pDesc = (SOCATDESC*)pArg;
		m_mPivot = pDesc->mPivot;
	}
	else
		XMStoreFloat4x4(&m_mPivot, XMMatrixIdentity());


	return S_OK;
}






void CSockat::Tick(_float fTimeDelta, CTransform* pParentTran)
{
	Update_ParentPos(pParentTran);

	for (_uint i = 0; i < m_Parts.size(); ++i)
	{
		if (nullptr == m_Parts[i])
			continue;

		m_Parts[i]->Tick(fTimeDelta);
	}

}

void CSockat::LateTick(_float fTimeDelta, CRenderer* pRenderer)
{
	if (nullptr == pRenderer)
		return;

	for (_uint i = 0; i < m_Parts.size(); ++i)
	{
		if (nullptr == m_Parts[i])
			continue;

		m_Parts[i]->LateTick(fTimeDelta);
		pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[i]);
	}

}





HRESULT CSockat::Match_Enum_BoneName(char * pBoneName, _int iIndex)
{
	if (iIndex >= m_BoneNames.size())
	{
		m_BoneNames.resize(iIndex+1);
		m_Sockets.resize(iIndex + 1);
		m_Parts.resize(iIndex + 1);
		m_PartsName.resize(iIndex + 1);
	}

	string str = pBoneName;
	m_BoneNames[iIndex] = str;

	return S_OK;
}




HRESULT CSockat::Add_Sockat(_int eID, CModel* pModel, _tchar * cName, PARTSDESC PartsDesc)
{
	if (nullptr == pModel)
		return E_FAIL;

	if (m_BoneNames.empty() && eID <= m_BoneNames.size())
		return E_FAIL;

	string sBoneName = m_BoneNames[eID];
	
	CHierarchyNode*		pSocket = pModel->Get_HierarchyNode(sBoneName.data());
	if (nullptr == pSocket)
		return E_FAIL;



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject*		pGameObject = pGameInstance->Clone_GameObject(cName, &PartsDesc);
	if (nullptr == pGameObject)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);


	char cModelName[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, PartsDesc.m_szModelName, MAX_PATH, cModelName, MAX_PATH, NULL, NULL);
	string sPartsName = cModelName;

	m_PartsName[eID] = sPartsName;
	Add_Child(pGameObject, pSocket , eID);
	

	return S_OK;
}







HRESULT CSockat::Remove_Sockat(_int eID)
{
	if (eID >= m_Parts.size())
			return E_FAIL;


	CGameObject* pParts = m_Parts[eID];
	if (nullptr == pParts)
		return S_OK;

	m_Parts[eID] = nullptr;
	m_Sockets[eID] = nullptr;
	m_PartsName[eID] = "";

	Safe_Release(pParts);

	return S_OK;
}

HRESULT CSockat::Remove_Sockat_If(_int eID, string sName)
{
	if (eID >= m_Parts.size())
		return E_FAIL;
	
	CGameObject* pParts = m_Parts[eID];
	if (nullptr == pParts)
		return S_OK;

	// 만약에 해당 아이템을 가지고 있다면 삭제
	if (sName != m_PartsName[eID])
		return S_OK;


	m_Parts[eID] = nullptr;
	m_Sockets[eID] = nullptr;
	m_PartsName[eID] = "";

	Safe_Release(pParts);

	return S_OK;
}






_bool CSockat::Check_Sockat(_int eID)
{
	if (eID >= m_Parts.size())
		return false;

	if (nullptr != m_Parts[eID])
		return false;
	
	return true;
}

_bool CSockat::Check_IsHaveSocket(_int eID, string sName)
{
	if (eID >= m_Parts.size())
		return false;

	if (nullptr == m_Parts[eID])
		return false;

	if (sName == m_PartsName[eID])
		return true;

	return false;
}








HRESULT CSockat::Add_Child(CGameObject * pObj, CHierarchyNode * pHierarchyNode, _int eID)
{
	if (nullptr == pObj || nullptr == pHierarchyNode)
		return E_FAIL;

	m_Parts[eID] = pObj;
	m_Sockets[eID] = pHierarchyNode; 

	return S_OK;
}

void CSockat::Update_ParentPos(CTransform* pParentTran)
{

	for (_int i = 0; i < m_Sockets.size(); ++i)
	{
		if (nullptr == m_Sockets[i])
			continue;

		_matrix mParent = m_Sockets[i]->Get_OffSetMatrix()
			* m_Sockets[i]->Get_CombinedTransformation()
			* XMLoadFloat4x4(&m_mPivot)
			* pParentTran->Get_WorldMatrix();

		m_Parts[i]->SetUp_State(mParent);
	}

}








CSockat * CSockat::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSockat*			pInstance = new CSockat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSockat"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CSockat::Clone(void * pArg)
{
	CSockat*			pInstance = new CSockat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSockat"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CSockat::Free()
{
	__super::Free();

	for (_int i = 0; i < m_Parts.size(); ++i)
		Safe_Release(m_Parts[i]);

}
