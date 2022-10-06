#include "..\Public\Sockat.h"

#include "GameObject.h"
#include "Renderer.h"
#include "HierarchyNode.h"
#include "Transform.h"

CSockat::CSockat(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CSockat::CSockat(const CSockat & rhs)
	: CComponent(rhs)
{

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

	for (auto& pPart : m_Parts)
		pPart->Tick(fTimeDelta);
}

void CSockat::LateTick(_float fTimeDelta, CRenderer* pRenderer)
{
	if (nullptr == pRenderer)
		return;

	for (auto& pPart : m_Parts)
		pPart->LateTick(fTimeDelta);

	for (auto& pPart : m_Parts)
		pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, pPart);
}


HRESULT CSockat::Add_Child(CGameObject * pObj, CHierarchyNode * pHierarchyNode)
{
	if (nullptr == pObj || nullptr == pHierarchyNode)
		return E_FAIL;

	m_Parts.push_back(pObj);
	m_Sockets.push_back(pHierarchyNode);

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
