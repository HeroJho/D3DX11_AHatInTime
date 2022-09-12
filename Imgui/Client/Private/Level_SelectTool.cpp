#include "stdafx.h"
#include "..\Public\Level_SelectTool.h"

#include "GameInstance.h"
#include "Camera_Free.h"


CLevel_SelectTool::CLevel_SelectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_SelectTool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;



	return S_OK;
}

void CLevel_SelectTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CLevel_SelectTool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	SetWindowText(g_hWnd, TEXT("선택 레벨임"));

	return S_OK;
}

CLevel_SelectTool * CLevel_SelectTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_SelectTool*		pInstance = new CLevel_SelectTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_SelectTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_SelectTool::Free()
{
	__super::Free();

}


