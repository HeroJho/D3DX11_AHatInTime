#include "stdafx.h"
#include "..\Public\MultiThread.h"

#include "GameInstance.h"

#include "DataManager.h"
#include "MeshManager.h"

#include "Cell.h"



CMultiThread::CMultiThread(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY MultiLoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CMultiThread*		pLoader = (CMultiThread*)pArg;

	EnterCriticalSection(&pLoader->Get_CS());

	switch (pLoader->Get_Type())
	{
	case CMultiThread::LOADING_READYNEIGHBOR:
		pLoader->Ready_Neighbor();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CS());


	return 0;
}

HRESULT CMultiThread::Initialize(LOADINGTYPE eLoadingType)
{
	m_eLoadingType = eLoadingType;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, MultiLoadingMain, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CMultiThread::Ready_Neighbor()
{
	// Cell을 삭제하면서 이웃 인덱스가 엉킨다. 다시 정렬한다.
	vector<CCell*>* pTempCells = CMeshManager::Get_Instance()->Get_Cells();
	vector<CCell*> Cells;
	for (auto& pCell : *pTempCells)
	{
		if (nullptr == pCell)
			continue;

		pCell->Set_Index(Cells.size());
		Cells.push_back(pCell);
	}



	m_iNumReadyNeighborMax = Cells.size();

	for (auto& pSourCell : Cells)
	{
		if (nullptr == pSourCell)
			continue;

		++m_iNumReadyNeighbor;

		for (auto& pDestCell : Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				// 너(Dest)는 내(Sour) AB이웃. 
				pSourCell->Set_NeighborIndex(CCell::LINE_AB, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_BC, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_CA, pDestCell);
			}
		}
	}


	m_isFinished = true;

	return S_OK;
}






CMultiThread * CMultiThread::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LOADINGTYPE eLoadingType)
{
	CMultiThread*		pInstance = new CMultiThread(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eLoadingType)))
	{
		MSG_BOX(TEXT("Failed To Created : CMultiThread"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMultiThread::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);


}
