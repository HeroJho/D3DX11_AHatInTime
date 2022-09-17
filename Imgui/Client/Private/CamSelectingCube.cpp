#include "stdafx.h"
#include "..\Public\CamSelectingCube.h"

#include "GameInstance.h"
#include "CamManager.h"

CCamSelectingCube::CCamSelectingCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCamSelectingCube::CCamSelectingCube(const CCamSelectingCube & rhs)
	: CGameObject(rhs)
{
}


HRESULT CCamSelectingCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamSelectingCube::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	COLORCUBEDESC* pDesc = (COLORCUBEDESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 0.f, 0.f, 0.f, 1.f });

	m_vRGB = pDesc->vColor;


	return S_OK;
}

void CCamSelectingCube::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);





	if (pGameInstance->Key_Pressing(DIK_UP))
		m_pTransformCom->Go_Dir(_vector{ 0.f, 0.f, 1.f, 0.f}, 20.f, fTimeDelta);
	else if (pGameInstance->Key_Pressing(DIK_DOWN))
		m_pTransformCom->Go_Dir(_vector{ 0.f, 0.f, -1.f, 0.f }, 20.f, fTimeDelta);
	else if (pGameInstance->Key_Pressing(DIK_LEFT))
		m_pTransformCom->Go_Dir(_vector{ -1.f, 0.f, 0.f, 0.f }, 20.f, fTimeDelta);
	else if (pGameInstance->Key_Pressing(DIK_RIGHT))
		m_pTransformCom->Go_Dir(_vector{ 1.f, 0.f, 0.f, 0.f }, 20.f, fTimeDelta);
	else if (pGameInstance->Key_Pressing(DIK_U))
		m_pTransformCom->Go_Dir(_vector{ 0.f, 1.f, 0.f, 0.f }, 20.f, fTimeDelta);
	else if (pGameInstance->Key_Pressing(DIK_J))
		m_pTransformCom->Go_Dir(_vector{ 0.f, -1.f, 0.f, 0.f }, 20.f, fTimeDelta);
	




	Safe_Release(pGameInstance);
}

void CCamSelectingCube::LateTick(_float fTimeDelta)
{

	if (CCamManager::Get_Instance()->Get_ShowCube())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CCamSelectingCube::Render()
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




_float3 CCamSelectingCube::Get_Pos()
{
	_float3 vTempPos;
	XMStoreFloat3(&vTempPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return vTempPos;
}
void CCamSelectingCube::Set_Pos(_vector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}



HRESULT CCamSelectingCube::SetUp_Components()
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

	TransformDesc.fSpeedPerSec = 25.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamSelectingCube::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);



	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vRGB, sizeof(_float4))))
		return E_FAIL;


	return S_OK;
}

CCamSelectingCube * CCamSelectingCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamSelectingCube*		pInstance = new CCamSelectingCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamSelectingCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamSelectingCube::Clone(void* pArg)
{
	CCamSelectingCube*		pInstance = new CCamSelectingCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCamSelectingCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamSelectingCube::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

