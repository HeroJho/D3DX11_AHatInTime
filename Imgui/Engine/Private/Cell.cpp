#include "..\Public\Cell.h"
#include "VIBuffer_Cell.h"
#include "Shader.h"
#include "PipeLine.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3 * pPoints, _int iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	_vector		vLine[LINE_END];

	vLine[LINE_AB] = XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]);
	vLine[LINE_BC] = XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]);
	vLine[LINE_CA] = XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]);

	m_vNormal[LINE_AB] = _float3(XMVectorGetZ(vLine[LINE_AB]) * -1.f, 0.f, XMVectorGetX(vLine[LINE_AB]));
	m_vNormal[LINE_BC] = _float3(XMVectorGetZ(vLine[LINE_BC]) * -1.f, 0.f, XMVectorGetX(vLine[LINE_BC]));
	m_vNormal[LINE_CA] = _float3(XMVectorGetZ(vLine[LINE_CA]) * -1.f, 0.f, XMVectorGetX(vLine[LINE_CA]));

	m_iIndex = iIndex;

#ifdef _DEBUG


	// 외적한다.
	_vector vNormal = XMVector3Normalize(XMVector3Cross(vLine[LINE_AB], vLine[LINE_BC]));
	// vNormal *= -1.f;
	vNormal = XMVector3Normalize(vNormal);
	XMStoreFloat3(&m_vNor ,vNormal);

	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	//m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElements);
	//if (nullptr == m_pShader)
	//	return E_FAIL;
#endif

	return S_OK;
}



_int CCell::Get_NumNeighbor()
{
	_int iNum = 0;
	for (_uint i = 0; i < 3; ++i)
	{
		if (-1 != m_iNeighborIndex[i])
			++iNum;
	}


	return iNum;
}



_bool CCell::Compare(const _float3 & vSourPoint, const _float3 & vDestPoint)
{
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vDestPoint)))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vDestPoint)))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vDestPoint)))
			return true;
	}

	return false;
}

#ifdef _DEBUG
HRESULT CCell::Render_Cell()
{

	m_pVIBuffer->Render();

	return S_OK;
}
#endif // _DEBUG

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints, _int iIndex)
{
	CCell*			pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX(TEXT("Failed To Created : CCell"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
