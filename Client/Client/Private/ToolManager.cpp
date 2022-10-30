#include "stdafx.h"
#include "..\Public\ToolManager.h"

#include "GameInstance.h"

#include "Level_Loading.h"

IMPLEMENT_SINGLETON(CToolManager)

CToolManager::CToolManager()
{

}

HRESULT CToolManager::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);


	for (_uint i = 0; i < TIME_END; ++i)
		m_fTimeRatios[i] = 1.f;


	return S_OK;
}

HRESULT CToolManager::Change_Level(LEVEL eLevel)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;

	if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

_float3 CToolManager::GetBesierPos(_float3 vPos1, _float3 vPos2, _float fT)
{
	_float3 temp;
	XMStoreFloat3(&temp, (1 - fT)* XMLoadFloat3(&vPos1) + fT*XMLoadFloat3(&vPos2));
	return temp;
}

_float3 CToolManager::GetBesierPos(_float3 vPos1, _float3 vPos2, _float3 vPos3, _float fT)
{
	_float3 temp;
	XMStoreFloat3(&temp, (1 - fT)*(1 - fT)*XMLoadFloat3(&vPos1) + 2 * (1 - fT)*fT*XMLoadFloat3(&vPos2) + fT*fT*XMLoadFloat3(&vPos3));
	return temp;
}

_float3 CToolManager::GetBesierPos(_float3 vPos1, _float3 vPos2, _float3 vPos3, _float3 vPos4, _float fT)
{
	_float3 temp;
	XMStoreFloat3(&temp, (1 - fT)*(1 - fT)*(1 - fT)*XMLoadFloat3(&vPos1) + 3 * (1 - fT)*(1 - fT)*fT*XMLoadFloat3(&vPos2) + 3 * (1 - fT) *fT *fT *XMLoadFloat3(&vPos3) + fT*fT*fT*XMLoadFloat3(&vPos4));
	return temp;
}

_tchar* CToolManager::Get_ManagedTChar()
{
	_tchar* pTemp = new _tchar[MAX_PATH];
	ZeroMemory(pTemp, sizeof(_tchar) * MAX_PATH);
	m_ManagedTChar.push_back(pTemp);

	return pTemp;
}

char* CToolManager::Get_ManagedChar()
{
	char* pTemp = new char[MAX_PATH];
	ZeroMemory(pTemp, sizeof(char) * MAX_PATH);
	m_ManagedChar.push_back(pTemp);

	return pTemp;
}

_float CToolManager::Get_TimeRatio(TIMETAG eTag)
{
	switch (eTag)
	{
	case Client::CToolManager::TIME_PLAYER:
		return m_fTimeRatios[TIME_PLAYER];
	case Client::CToolManager::TIME_MONSTER:
		return m_fTimeRatios[TIME_MONSTER];
	case Client::CToolManager::TIME_EM:
		return m_fTimeRatios[TIME_EM];
	}
}

void CToolManager::Set_TimeRatio(TIMETAG eTag, _float fTimeRatio)
{
	switch (eTag)
	{
	case Client::CToolManager::TIME_PLAYER:
		m_fTimeRatios[TIME_PLAYER] = fTimeRatio;
	case Client::CToolManager::TIME_MONSTER:
		m_fTimeRatios[TIME_MONSTER] = fTimeRatio;
	case Client::CToolManager::TIME_EM:
		m_fTimeRatios[TIME_EM] = fTimeRatio;
	}
}

void CToolManager::Set_WithOutPlayer(_float fTimeRatio)
{
	Set_TimeRatio(TIME_MONSTER, fTimeRatio);
	Set_TimeRatio(TIME_EM, fTimeRatio);
}

void CToolManager::Set_All(_float fTimeRatio)
{
	for (_uint i = 0; i < TIME_END; ++i)
	{
		Set_TimeRatio((TIMETAG)i, fTimeRatio);
	}
}







void CToolManager::ClearManagedChar()
{
	for (auto& pTChar : m_ManagedTChar)
		Safe_Delete(pTChar);
	for (auto& pChar : m_ManagedChar)
		Safe_Delete(pChar);
}

void CToolManager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ClearManagedChar();
}

