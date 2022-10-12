#include "stdafx.h"
#include "..\Public\ClickedFreeVectexCube.h"

#include "GameInstance.h"


CClickedFreeVectexCube::CClickedFreeVectexCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CClickedFreeVectexCube::CClickedFreeVectexCube(const CClickedFreeVectexCube & rhs)
	: CGameObject(rhs)
{
}





HRESULT CClickedFreeVectexCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CClickedFreeVectexCube::Initialize(void * pArg)
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

	m_vRGB = pDesc->vColor;

	return S_OK;
}

void CClickedFreeVectexCube::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(DIK_UP))
	{

	}
	else if (pGameInstance->Key_Pressing(DIK_DOWN))
	{

	}


	RELEASE_INSTANCE(CGameInstance);
}

void CClickedFreeVectexCube::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CClickedFreeVectexCube::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}






void CClickedFreeVectexCube::Set_Pos(_float3 vPos)
{
	_vector vTempPos = XMLoadFloat3(&vPos);
	vTempPos = XMVectorSetW(vTempPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTempPos);
}

_float3 CClickedFreeVectexCube::Get_Pos()
{
	_float3 vTempPos;
	ZeroMemory(&vTempPos, sizeof(_float3));
	XMStoreFloat3(&vTempPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return vTempPos;
}







HRESULT CClickedFreeVectexCube::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_ColorCube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For. Com_Shader*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_ColorCube"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(TransformDesc));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CClickedFreeVectexCube::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vRGB, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}






CClickedFreeVectexCube * CClickedFreeVectexCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CClickedFreeVectexCube*		pInstance = new CClickedFreeVectexCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CClickedFreeVectexCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CClickedFreeVectexCube::Clone(void* pArg)
{
	CClickedFreeVectexCube*		pInstance = new CClickedFreeVectexCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CClickedFreeVectexCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CClickedFreeVectexCube::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

