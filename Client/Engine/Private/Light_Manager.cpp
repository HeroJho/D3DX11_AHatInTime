#include "..\Public\Light_Manager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

const LIGHTDESC * CLight_Manager::Get_LightDesc(_uint iIndex)
{
	auto	iter = m_Lights.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();
}

_float4x4 CLight_Manager::Get_ShadowLightViewMatrix()
{
	_float4x4 mMatirx;
	XMStoreFloat4x4(&mMatirx, XMMatrixTranspose(XMLoadFloat4x4(&m_mShadowLightViewMatrix)));
	return mMatirx;
}


HRESULT CLight_Manager::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	CLight*		pLight = CLight::Create(pDevice, pContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc, CLight ** Out_pLight)
{
	CLight*		pLight = CLight::Create(pDevice, pContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);
	*Out_pLight = pLight;

	return S_OK;
}

HRESULT CLight_Manager::Remove_Light(CLight * pLight)
{
	
	for (list<class CLight*>::iterator iter = m_Lights.begin(); iter != m_Lights.end(); )
	{
		if (*iter == pLight)
		{
			Safe_Release(*iter);
			m_Lights.erase(iter);
			break;
		}
		else
			++iter;
	}

	return S_OK;
}

void CLight_Manager::Clear_Light()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}

HRESULT CLight_Manager::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	
	_vector		vLightEye = XMVectorSet(24.f, 100.9f, 82.95f, 1.f);
	_vector		vLightAt = XMVectorSet(-56.7f, 4.7f, 74.09f, 1.f);
	_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMStoreFloat4x4(&m_mShadowLightViewMatrix, XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp));


	for (auto& pLight : m_Lights)
		pLight->Render(pShader, pVIBuffer);

	return S_OK;
}

void CLight_Manager::Free()
{
	Clear_Light();
}
