#include "stdafx.h"
#include "..\Public\Sky.h"

#include "GameInstance.h"

CSky::CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSky::CSky(const CSky & rhs)
	: CGameObject(rhs)
{
}


HRESULT CSky::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSky::Initialize(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;	

	return S_OK;
}

void CSky::Tick(_float fTimeDelta)
{
		
}

void CSky::LateTick(_float fTimeDelta)
{
	_matrix		ViewMatrix;

	ViewMatrix = CGameInstance::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);


    ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ViewMatrix.r[3]);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSky::Render()
{
	//if (FAILED(m_pTransformCom->Bind_WorldMatrix()))
	//	return E_FAIL;	
	//
	//if (FAILED(m_pTextureCom->Bind_Texture(2)))
	//	return E_FAIL;

	//if (FAILED(Set_RenderState()))
	//	return E_FAIL;

	//m_pVIBufferCom->Render();

	//if (FAILED(Reset_RenderState()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CSky::Set_RenderState()
{
	if (nullptr == m_pDevice || nullptr == m_pContext)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	return S_OK;
}

HRESULT CSky::Reset_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);

	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}

HRESULT CSky::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
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

CSky * CSky::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSky*		pInstance = new CSky(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSky"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSky::Clone(void* pArg)
{
	CSky*		pInstance = new CSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CSky"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

