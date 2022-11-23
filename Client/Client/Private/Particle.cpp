#include "stdafx.h"
#include "..\Public\Particle.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "FlaskLight.h"

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


	// m_pTransformCom->Set_Look(XMLoadFloat3(&m_Desc.vLook));


	_vector vVLook = XMLoadFloat3(&m_Desc.vLook);
	if (0.01f <  XMVectorGetX(XMVector3Length(vVLook)))
	{
		_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vVLook);
		_vector		vUp = XMVector3Cross(vVLook, vRight);
		_matrix mRoteMatrix = XMMatrixIdentity();
		mRoteMatrix.r[0] = XMVectorSetW(XMVector3Normalize(vRight), 0.f);
		mRoteMatrix.r[1] = XMVectorSetW(XMVector3Normalize(vUp), 0.f);
		mRoteMatrix.r[2] = XMVectorSetW(XMVector3Normalize(vVLook), 0.f);
		mRoteMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		_vector vDir = XMLoadFloat3(&m_Desc.vDir);
		XMStoreFloat3(&m_Desc.vDir, XMVector3TransformNormal(vDir, mRoteMatrix));
		m_pTransformCom->Set_Look(mRoteMatrix.r[2]);
	}
	

	// 바라보고 회전
		m_pTransformCom->RotationStay(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_Desc.vRoation.x,
		XMVectorSet(0.f, 1.f, 0.f, 0.f), m_Desc.vRoation.y,
		XMVectorSet(0.f, 0.f, 1.f, 0.f), m_Desc.vRoation.z);

	if (m_Desc.bGravity)
		m_pTransformCom->Jump(m_Desc.fJumpPower);


	XMStoreFloat3(&m_vRotDir, XMVector3Normalize(XMVectorSet(m_Desc.vRoation.x, m_Desc.vRoation.y, m_Desc.vRoation.z, 0.f)));


	m_fRotationSpeed = CToolManager::Get_Instance()->Get_RendomNum(150.f, 300.f);
	if (_int(m_fRotationSpeed) % 2)
		m_bRotC = true;
	else
		m_bRotC = false;

	if (!lstrcmp(TEXT("Prototype_Component_Texture_Star"), m_Desc.cModelTag))
	{
		//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		//CFlaskLight::FLASKLIGHTDESC LightDesc;
		//XMStoreFloat3(&LightDesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		//LEVEL eLevel = CToolManager::Get_Instance()->Get_CulLevel();
		//pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_FlaskLight"), eLevel, TEXT("Layer_Light"), &LightDesc);
		//RELEASE_INSTANCE(CGameInstance);
	}

	return S_OK;
}








void CParticle::Tick(_float fTimeDelta)
{

	switch (m_Desc.eType)
	{
	case TYPE_MODLE:
		Tick_Model(fTimeDelta);
		break;
	case TYPE_TEXTURE:
		Tick_Texture(fTimeDelta);
		break;
	default:
		break;
	}


}

void CParticle::Tick_Model(_float fTimeDelta)
{


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	// 몇 초 동안
	m_fLifeTimeAcc += fTimeDelta;
	if (m_Desc.fLifeTime < m_fLifeTimeAcc)
		Set_Dead(true);


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

	if (lstrcmp(TEXT("SprintParticle"), m_Desc.cModelTag))
	{
		if (m_bRotC)
			m_pTransformCom->Turn(XMVectorSetW(XMLoadFloat3(&m_vRotDir), 0.f), m_fRotationSpeed * 0.01f, fTimeDelta);
		else
			m_pTransformCom->Turn(-XMVectorSetW(XMLoadFloat3(&m_vRotDir), 0.f), m_fRotationSpeed * 0.01f, fTimeDelta);
	}

	RELEASE_INSTANCE(CGameInstance);


}

void CParticle::Tick_Texture(_float fTimeDelta)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	// 몇 초 동안
	m_fLifeTimeAcc += fTimeDelta;
	if (m_Desc.fLifeTime < m_fLifeTimeAcc)
		Set_Dead(true);


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

	switch (m_Desc.eType)
	{
	case TYPE_MODLE:
		LateTick_Model(fTimeDelta);
		break;
	case TYPE_TEXTURE:
		LateTick_Texture(fTimeDelta);
		break;
	default:
		break;
	}

}

void CParticle::LateTick_Model(_float fTimeDelta)
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
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

void CParticle::LateTick_Texture(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// 빌보드
	_matrix mCam = pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);
	_vector vCamPos = mCam.r[3];

	if (m_bRotC)
		m_fAnigleAcc += m_fRotationSpeed * fTimeDelta;
	else
		m_fAnigleAcc -= m_fRotationSpeed * fTimeDelta;

	m_pTransformCom->LookAtInv(vCamPos, m_fAnigleAcc);

	// 중력
	if (m_Desc.bGravity)
		m_pTransformCom->Tick_Gravity(fTimeDelta, nullptr, m_Desc.fGravityValue);


	// 소팅
	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	if (true == isDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance);
}








HRESULT CParticle::Render()
{

	switch (m_Desc.eType)
	{
	case TYPE_MODLE:
		if (FAILED(Render_Model()))
			return E_FAIL;
		break;
	case TYPE_TEXTURE:
		if (FAILED(Render_Texture()))
			return E_FAIL;
		break;
	default:
		break;
	}


}

HRESULT CParticle::Render_Model()
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


	_uint iPassIndex = 0;
	if (!lstrcmp(TEXT("SprintParticle"), m_Desc.cModelTag) || !lstrcmp(TEXT("SmokeParticle"), m_Desc.cModelTag))
	{
		iPassIndex = 10;
	}


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CParticle::Render_Texture()
{
	if (nullptr == m_pVIBufferCom ||
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

	_uint iPassIndex = 1;
	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (!lstrcmp(TEXT("Prototype_Component_Texture_Star"), m_Desc.cModelTag))
	{
		iPassIndex = 3;

		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture1", 1)))
			return E_FAIL;
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture2", 2)))
			return E_FAIL;
	}
	else if (!lstrcmp(TEXT("Prototype_Component_Texture_star_shuriken"), m_Desc.cModelTag))
	{
		iPassIndex = 4;
	}
	else if (!lstrcmp(TEXT("Prototype_Component_Texture_T_FX_Flare_01"), m_Desc.cModelTag))
	{
		iPassIndex = 5;
	}


	if (FAILED(m_pShaderCom->Begin(iPassIndex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}











HRESULT CParticle::Ready_Components()
{

	switch (m_Desc.eType)
	{
	case TYPE_MODLE:
		if (FAILED(Ready_Model_Components()))
			return E_FAIL;
		break;
	case TYPE_TEXTURE:
		if (FAILED(Ready_Texture_Components()))
			return E_FAIL;
		break;
	default:
		break;
	}


	return S_OK;
}

HRESULT CParticle::Ready_Model_Components()
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
}

HRESULT CParticle::Ready_Texture_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_Desc.cModelTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
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


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
