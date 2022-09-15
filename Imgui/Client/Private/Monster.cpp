#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CLandObject(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{
	__super::LANDDESC		LandDesc;
	ZeroMemory(&LandDesc, sizeof(__super::LANDDESC));

	LandDesc.iTerrainLevelIndex = LEVEL_CAMTOOL;
	LandDesc.pLayerTag = TEXT("Layer_BackGround");
	LandDesc.iTerrainObjectIndex = 0;
	LandDesc.pTerrainBufferComTag = TEXT("Com_VIBuffer");

	if (FAILED(CLandObject::Initialize(&LandDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ (rand() % 10.f), 0.5f, 3.0f, 1.0f });

	return S_OK;
}

void CMonster::Tick(_float fTimeDelta)
{

	// m_pTransformCom->Chase(m_vTargetPos, fTimeDelta);
	m_pTransformCom->LookAt_ForLandObject(_vector{ 1.f, 50.f, 0.f });
	
}

void CMonster::LateTick(_float fTimeDelta)
{
	_matrix ViewMatrix;


	ViewMatrix = CGameInstance::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	/* 카메라의 월드행렬이다. */
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0]);
	// m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0]);
	// // m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2]);


	// SetUp_OnTerrain(m_pTransformCom, 0.5f);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMonster::Render()
{
	//if (FAILED(m_pTransformCom->Bind_WorldMatrix()))
	//	return E_FAIL;	
	//
	//if (FAILED(m_pTextureCom->Bind_Texture(0)))
	//	return E_FAIL;

	if (FAILED(Set_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Set_RenderState()
{
	if (nullptr == m_pDevice || nullptr == m_pDeviceContext)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);	
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	return S_OK;
}

HRESULT CMonster::Reset_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

HRESULT CMonster::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
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

CMonster * CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster*		pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonster::Clone(void* pArg)
{
	CMonster*		pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

