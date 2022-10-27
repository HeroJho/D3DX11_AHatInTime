#include "stdafx.h"
#include "..\Public\LookCube.h"

#include "GameInstance.h"
#include "CamManager.h"

CLookCube::CLookCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLookCube::CLookCube(const CLookCube & rhs)
	: CGameObject(rhs)
{
}


HRESULT CLookCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLookCube::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	COLORCUBEDESC* pDesc = (COLORCUBEDESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 0.f, 0.f, 0.f, 1.f });

	_vector vTempPos = XMLoadFloat3(&(pDesc->vPos));
	vTempPos = XMVectorSetW(vTempPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTempPos);
	m_pTransformCom->Set_Scale(XMLoadFloat3(&(pDesc->vScale)));

	return S_OK;
}

void CLookCube::Tick(_float fTimeDelta)
{

}

void CLookCube::LateTick(_float fTimeDelta)
{

}

HRESULT CLookCube::Render()
{

	return S_OK;
}

void CLookCube::Set_Pos(_float3 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
}

_float3 CLookCube::Get_Pos()
{
	_float3 vTempPos;
	ZeroMemory(&vTempPos, sizeof(_float3));
	XMStoreFloat3(&vTempPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return vTempPos;
}

CLookCube::CAMDATA CLookCube::Get_SaveDATA()
{
	CAMDATA Data;
	ZeroMemory(&Data, sizeof(CAMDATA));

	XMStoreFloat3(&Data.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	Data.fSpeed = m_fSpeedTime;
	Data.iLinkIndex = m_iLinkIndex;

	return Data;
}
void CLookCube::Set_SaveDATA(CDataManager::CAMDATA * pData)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pData->vPos), 1.f));
	m_fSpeedTime = pData->fSpeed;
	m_iLinkIndex = pData->iLinkIndex;
}


HRESULT CLookCube::SetUp_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(TransformDesc));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}


CLookCube * CLookCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLookCube*		pInstance = new CLookCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLookCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLookCube::Clone(void* pArg)
{
	CLookCube*		pInstance = new CLookCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CLookCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLookCube::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}

