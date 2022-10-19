#include "..\Public\GameObject.h"
#include "GameInstance.h"
#include "Component.h"


CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice); 
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice), m_pContext(rhs.m_pContext)
{
	Safe_AddRef(m_pDevice); 
	Safe_AddRef(m_pContext);
}

CComponent * CGameObject::Get_ComponentPtr(const _tchar * pComponentTag)
{
	CComponent*		pComponent = Find_Component(pComponentTag);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;	
}

COBB * CGameObject::Get_StaticOBB()
{
	if (m_Colliders.empty())
		return nullptr;

	CCollider* pCol = m_Colliders.front();
	if (CCollider::TYPE_OBB != pCol->Get_ColliderType())
		return nullptr;

	return (COBB*)pCol;
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{
	return S_OK;
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::LateTick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::Tick_Col(_fmatrix TransformMatrix, CNavigation* pNavi, CTransform* pTran)
{
	for (auto& pCollider : m_Colliders)
		pCollider->Update(TransformMatrix, pNavi, pTran);
}

void CGameObject::Render_Col()
{
	for (auto& pCollider : m_Colliders)
		pCollider->Render();
}

HRESULT CGameObject::AddCollider(CCollider::TYPE eType, CCollider::tagColliderDesc Desc)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider*			pCollider = nullptr;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		pCollider = (CCollider*)pGameInstance->Clone_Component(0, TEXT("Prototype_Component_Collider_AABB"), &Desc);
		break;
	case CCollider::TYPE_OBB:
		pCollider = (CCollider*)pGameInstance->Clone_Component(0, TEXT("Prototype_Component_Collider_OBB"), &Desc);
		break;
	case CCollider::TYPE_SPHERE:
		pCollider = (CCollider*)pGameInstance->Clone_Component(0, TEXT("Prototype_Component_Collider_Sphere"), &Desc);
		break;
	}

	if (nullptr == pCollider)
		return E_FAIL;

	m_Colliders.push_back(pCollider);


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	CComponent*			pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);	

	return S_OK;
}


CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	for (auto& pCollider : m_Colliders)
		Safe_Release(pCollider);
	
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
