#include "..\Public\Picking.h"
#include "Transform.h"

#include "PipeLine.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}


HRESULT CPicking::Initialize(HWND hWnd, _uint iWinCX, _uint iWinCY, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	m_hWnd = hWnd;

	m_iWinCX = iWinCX;

	m_iWinCY = iWinCY;

	return S_OK;
}

void CPicking::Tick()
{
	POINT			ptMouse;

	GetCursorPos(&ptMouse);

	ScreenToClient(m_hWnd, &ptMouse);

	_float3			vMousePos;

	/* 투영공간상의 마우스 좌표를 구한다. */
	vMousePos.x = _float(ptMouse.x / (m_iWinCX * 0.5f) - 1);
	vMousePos.y = _float(ptMouse.y / (m_iWinCY * -0.5f) + 1);
	vMousePos.z = 0.f;

	XMVECTOR vVectorMousePos = XMLoadFloat3(&vMousePos);
	
	/* 뷰스페이스 상의 좌표를 구한다. */
	_matrix		ProjMatrixInv;
	ProjMatrixInv = CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	ProjMatrixInv = XMMatrixInverse(nullptr, ProjMatrixInv);
	vVectorMousePos = XMVector3TransformCoord(vVectorMousePos, ProjMatrixInv);


	/* 뷰스페이스 상의 마우스 레이와 레이포스를 구하자. */
	XMVECTOR vVectorRayPos { 0.f, 0.f, 0.f, 0.f };
	XMVECTOR vVectorRayDir = vVectorMousePos - vVectorRayPos;
	

	/* 월드스페이스 상의 마우스 레이와 레이포스를 구하자. */	
	_matrix ViewMatrixInv;
	ViewMatrixInv = CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	ViewMatrixInv = XMMatrixInverse(nullptr, ViewMatrixInv);
	vVectorMousePos = XMVector3TransformCoord(vVectorRayPos, ViewMatrixInv);
	vVectorRayDir = XMVector3TransformNormal(vVectorRayDir, ViewMatrixInv);

	XMStoreFloat3(&m_vRayPos, vVectorMousePos);
	XMStoreFloat3(&m_vRayDir, vVectorRayDir);

}

void CPicking::Compute_LocalRayInfo(_float3 * pRayDir, _float3 * pRayPos, CTransform * pTransform)
{
	_matrix		WorldMatrixInv = pTransform->Get_WorldMatrixInverse();
	
	XMStoreFloat3(pRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), WorldMatrixInv));
	XMStoreFloat3(pRayDir,XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), WorldMatrixInv));
}

void CPicking::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
