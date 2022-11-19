#include "stdafx.h"
#include "..\Public\Particle.h"
#include "GameInstance.h"

#include "MapManager.h"
#include "MeshManager.h"
#include "ToolManager.h"


CParticle::CParticle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(m_Desc.cModelTag, sizeof(TCHAR) * MAX_PATH);

}

CParticle::CParticle(const CParticle & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(m_Desc.cModelTag, sizeof(TCHAR) * MAX_PATH);

}




HRESULT CParticle::Initialize_Prototype()
{



	return S_OK;
}

HRESULT CParticle::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_Desc, pArg, sizeof(PARTICLEDESC));


	if (FAILED(Ready_Components()))
		return E_FAIL;


	_vector vTotalPos = XMLoadFloat3(&m_Desc.vPos) + XMLoadFloat3(&m_Desc.vParentPos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vTotalPos, 1.f));
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_Desc.vScale));

	m_pTransformCom->Set_Look(XMLoadFloat3(&m_Desc.vLook));

	// 바라보고 회전
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_Desc.vRoation.x,
		XMVectorSet(0.f, 1.f, 0.f, 0.f), m_Desc.vRoation.y,
		XMVectorSet(0.f, 0.f, 1.f, 0.f), m_Desc.vRoation.z);

	if (m_Desc.bGravity)
		m_pTransformCom->Jump(m_Desc.fJumpPower);

	return S_OK;
}








void CParticle::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	// 몇 초 동안
	m_fLifeTimeAcc += fTimeDelta;
	if (m_Desc.fLifeTime < m_fLifeTimeAcc)
		Set_Dead();


	// 어느 방향으로 어느 속도로
	_vector vDir = XMLoadFloat3(&m_Desc.vDir);
	m_pTransformCom->Go_Dir(vDir, m_Desc.fSpeed, fTimeDelta);

	// 속도 감쇠
	m_fStopValueAcc += m_Desc.fStopValue * fTimeDelta;
	m_Desc.fSpeed -= m_fStopValueAcc;
	if (0.f > m_Desc.fSpeed)
	{
		m_Desc.fSpeed = 0.f;
		m_fStopValueAcc = 0.f;
	}



	// 크기 변화
	m_pTransformCom->Go_Scale(m_Desc.vScaleSpeed, fTimeDelta);


	RELEASE_INSTANCE(CGameInstance);
}

void CParticle::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	// 중력
	if (m_Desc.bGravity)
		m_pTransformCom->Tick_Gravity(fTimeDelta, nullptr, m_Desc.fGravityValue);




	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	RELEASE_INSTANCE(CGameInstance);
	if (true == isDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}








HRESULT CParticle::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);





	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}



	return S_OK;
}









HRESULT CParticle::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_Desc.cModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	return S_OK;
}






CParticle * CParticle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParticle*		pInstance = new CParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CParticle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParticle::Clone(void * pArg)
{
	CParticle*		pInstance = new CParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CParticle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
