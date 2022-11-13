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

HRESULT CToolManager::Change_Level()
{
	if (LEVEL_END == m_eLEVEL)
		return S_OK;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;

	if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_eLEVEL))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	m_eLEVEL = LEVEL_END;
	m_bLoading = true;

	return S_OK;
}

void CToolManager::Resul_Level(LEVEL eLevel)
{
	m_eLEVEL = eLevel;
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

_float CToolManager::Get_RendomNum(_float fMin, _float fMax)
{
	// 시드값을 얻기 위한 random_device 생성.
	std::random_device rd;

	// random_device 를 통해 난수 생성 엔진을 초기화 한다.
	std::mt19937 gen(rd());

	// 0 부터 99 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
	std::uniform_real_distribution<_float> dis(fMin, fMax);
	return dis(gen);
}

_int CToolManager::Get_RendomNum_Int(_int iMin, _int iMax)
{
	// 시드값을 얻기 위한 random_device 생성.
	std::random_device rd;

	// random_device 를 통해 난수 생성 엔진을 초기화 한다.
	std::mt19937 gen(rd());

	// 0 부터 99 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
	std::uniform_int_distribution<_int> dis(iMin, iMax);
	return dis(gen);
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
	case Client::CToolManager::TIME_CAM:
		return m_fTimeRatios[TIME_CAM];
	}
}

void CToolManager::Set_TimeRatio(TIMETAG eTag, _float fTimeRatio)
{
	switch (eTag)
	{
	case Client::CToolManager::TIME_PLAYER:
		m_fTimeRatios[TIME_PLAYER] = fTimeRatio;
		break;
	case Client::CToolManager::TIME_MONSTER:
		m_fTimeRatios[TIME_MONSTER] = fTimeRatio;
		break;
	case Client::CToolManager::TIME_EM:
		m_fTimeRatios[TIME_EM] = fTimeRatio;
		break;
	case Client::CToolManager::TIME_CAM:
		m_fTimeRatios[TIME_CAM] = fTimeRatio;
		break;
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





_uint CToolManager::Find_NaviIndex(_fvector vPos)
{
	return m_pNavi->Find_NaviIndex(vPos);
}

HRESULT CToolManager::Clone_Navi(LEVEL eLevel)
{

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CComponent*			pComponent = pGameInstance->Clone_Component(eLevel, TEXT("Prototype_Component_Navigation"));
	if (nullptr == pComponent)
		return E_FAIL;

	Safe_Release(m_pNavi);

	m_pNavi = (CNavigation*)pComponent;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CToolManager::Ready_CellCollision(CGameObject * pObj, _int* iIndexs, _int iSize)
{
	m_pNavi->Ready_CellCollision(pObj, iIndexs, iSize);
}







void CToolManager::ClearManagedChar()
{
	for (auto& pTChar : m_ManagedTChar)
		Safe_Delete(pTChar);
	for (auto& pChar : m_ManagedChar)
		Safe_Delete(pChar);
}


void CToolManager::Render_Fonts(const _tchar * pFontTag, const _tchar * pTextm, _float2 vPosition, _fvector vColor, _float fAngle, _float2 vOrigin, _float2 vScale, _bool bSentor)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_int iLenthNum = lstrlen(pTextm);

	_float fLeft = vScale.x * 10.f * ((_float)iLenthNum);
	_float fUp = vScale.y * 10.f;
	if (bSentor)
	{
		pGameInstance->Render_Fonts(pFontTag, pTextm,
			_float2(vPosition.x - fLeft, vPosition.y - fUp), vColor, fAngle, vOrigin, vScale);
	}
	else
	{
		pGameInstance->Render_Fonts(pFontTag, pTextm,
			_float2(vPosition.x, vPosition.y), vColor, fAngle, vOrigin, vScale);
	}


	RELEASE_INSTANCE(CGameInstance);
}

void CToolManager::Free()
{
	Safe_Release(m_pNavi);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ClearManagedChar();
}

