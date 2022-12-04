#include "stdafx.h"
#include "..\Public\Magic.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "ParticleManager.h"
#include "CamManager.h"

#include "Player.h"
#include "Camera_Free.h"
#include "FlaskLight.h"

#include "Flask_EX.h"
#include "Wind_Spiral.h"

CMagic::CMagic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CMagic::CMagic(const CMagic & rhs)
	: CGameObject(rhs)
{

}

HRESULT CMagic::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CMagic::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	MAGICDESC* pDesc = (MAGICDESC*)(pArg);
	
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vStartPos), 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(5.f, 5.f, 5.f, 1.f));

	m_vSentorPos = pDesc->vSentorPos;
	m_fGoTime = pDesc->fGoTime;

	m_sTag = "Tag_Magic";


	Set_State(STATE_IN);

	_float fX = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
	_float fY = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
	_float fZ = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
	XMStoreFloat3(&m_vRotationDir,  XMVector3Normalize(XMVectorSet(fX, fY, fZ, 0.f)));
	
	m_fRotationSpeed = CToolManager::Get_Instance()->Get_RendomNum(1.f, 3.f);

	return S_OK;
}

void CMagic::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);


	switch (m_eState)
	{
	case STATE_IN:
		Tick_In(fTimeDelta);
		break;
	case STATE_IDLE:
		Tick_Idle(fTimeDelta);
		break;
	case STATE_GO:
		Tick_Go(fTimeDelta);
		break;
	default:
		break;
	}

	Rotate_Sentor(fTimeDelta);
}

void CMagic::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (STATE_GO == m_eState)
	{
		m_pTransformCom->Tick_Gravity(fTimeDelta, nullptr, 3.f);

		_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
		Tick_Col(m_pTransformCom->Get_WorldMatrix());


		// ÂøÁö Çß´Ù¸é
		_float fTemp = 0.f;
		if (0.5f > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		{
			m_bOn = true;
			m_bEx = true;
		}

		if (m_bEx)
		{
			CFlask_EX::FLASKEXDESC Desc;
			XMStoreFloat3(&Desc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CGameInstance::Get_Instance()->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Flask_EX"), LEVEL_GAMEPLAY, TEXT("Layer_EX"), &Desc);


			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), Desc.vPos, _float3(0.f, 3.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(3.f, 3.f, 3.f), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 4.f, 15.f, false, 0.f, 0.f, 2.f,
				30, 0.f, 0.5f, 0.f, 0.f, 0.f, 0.1f, 0.f, 0.1f, 0.1f, _float3(-90.f, 0.f, -90.f), _float3(90.f, 0.f, 90.f), CParticle::TYPE_MODLE);
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), Desc.vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.f, 2.f, false, 0.f, 0.f, 2.f,
				5, 2.f, 0.5f, 0.f, 0.f, 0.f, 0.1f, 0.f, 0.1f, 0.1f, _float3(-10.f, 0.f, -10.f), _float3(10.f, 0.f, 10.f), CParticle::TYPE_MODLE);
			CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), Desc.vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(90.f, 0.f, 0.f), 0.1f, 4.f, false, 0.f, 0.f, 2.f,
				10, 0.f, 0.5f, 0.f, 0.f, 0.f, 0.1f, 0.f, 0.1f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(0.f, 360.f, 0.f), CParticle::TYPE_MODLE);




			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			LEVEL eLevel = CToolManager::Get_Instance()->Get_CulLevel();

			CWind_Spiral::WINDSPIRALDESC WindDesc;
			XMStoreFloat3(&WindDesc.vParPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			WindDesc.vLocalPos = _float3(0.f, 2.f, 0.f);
			WindDesc.vScale = _float3(5.f, 3.f, 5.f);
			WindDesc.fMaxScale = 8.f;
			WindDesc.fScaleSpeed = 2.f;
			WindDesc.fAngle = -90.f;
			pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Wind_Spiral"), eLevel, TEXT("Layer_Light"), &WindDesc);

			WindDesc.vLocalPos = _float3(0.f, 1.f, 0.f);
			WindDesc.vScale = _float3(3.f, 2.f, 3.f);
			WindDesc.fMaxScale = 6.f;
			WindDesc.fScaleSpeed = 2.f;
			WindDesc.fAngle = 90.f;
			pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Wind_Spiral"), eLevel, TEXT("Layer_Light"), &WindDesc);




			CCamManager::Get_Instance()->Get_Cam()->Start_Shake(0.25f, 10.f, 0.05f);


			CFlaskLight::FLASKLIGHTDESC LightDesc;
			XMStoreFloat3(&LightDesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_FlaskLight"), eLevel, TEXT("Layer_Light"), &LightDesc);
			RELEASE_INSTANCE(CGameInstance);


			pGameInstance->PlaySoundW(L"bombcake_explode.ogg", SOUND_SNATEFFECT, g_fEffectSound + 0.2f);


			Set_Dead(true);
		}
	}

	



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (STATE_GO == m_eState)
		pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_PROJECT, this);

	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	if (true == isDraw)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}


	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMagic::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix()));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	_bool bBlur = true;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bBlur", &bBlur, sizeof(_bool))))
		return E_FAIL;
	


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	return S_OK;

	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();

	return S_OK;
}

HRESULT CMagic::Render_ShadowDepth()
{

	if (nullptr == m_pShaderCom ||
		nullptr == m_pTransformCom)
		return E_FAIL;



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;



	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_ShadowLightViewMatrix(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(0), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 13)))
			return E_FAIL;
	}


	return S_OK;
}





void CMagic::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if (!strcmp("Attacked_Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
		m_bEx = true;
}

void CMagic::Set_State(STATE eState)
{
	if (eState != m_eState)
	{
		switch (eState)
		{
		case Client::CMagic::STATE_IN:
			break;
		case Client::CMagic::STATE_IDLE:
			break;
		case Client::CMagic::STATE_GO:
		{
			m_pTransformCom->Jump(4.f);

			_float fX = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
			_float fY = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
			_float fZ = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
			XMStoreFloat3(&m_vGoDir, XMVector3Normalize(XMVectorSet(fX, fY, fZ, 0.f)));
			m_fDirPow = CToolManager::Get_Instance()->Get_RendomNum(3.f, 8.f);
		}
			break;
		default:
			break;
		}
	}

	m_eState = eState;
}





void CMagic::Rotate_Sentor(_float fTimeDelta)
{
	m_pTransformCom->Turn(XMLoadFloat3(&m_vRotationDir), m_fRotationSpeed, fTimeDelta);
	m_pTransformCom->Turn_Sentor(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat3(&m_vSentorPos), 1.f, fTimeDelta);
}

void CMagic::Tick_In(_float fTimeDelta)
{
	_vector vSentorPos = XMLoadFloat3(&m_vSentorPos);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_vector vDir = vSentorPos - vMyPos;
	_vector vNorDir = XMVector3Normalize(vDir);
	_float fDis = XMVectorGetX(XMVector3Length(vDir));

	if (4.f > fDis)
		Set_State(STATE_IDLE);
	else
		m_pTransformCom->Go_Dir(vNorDir, 10.f, fTimeDelta);

}

void CMagic::Tick_Idle(_float fTimeDelta)
{
	m_fGoTimeAcc += fTimeDelta;
	if (m_fGoTime < m_fGoTimeAcc)
		Set_State(STATE_GO);

}

void CMagic::Tick_Go(_float fTimeDelta)
{
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vGoDir), m_fDirPow, fTimeDelta);
}







HRESULT CMagic::Ready_Components()
{

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_ParentTransform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_ParentTransform"), (CComponent**)&m_pParentTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("science_owlbrew_remade"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;




	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.2f, 0.2f, 0.2f);
	ColDesc.bIsStatic = true;
	strcpy(ColDesc.sTag, "Static_OBB");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.77f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.2f, 0.2f, 0.2f);
	strcpy(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

CMagic * CMagic::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMagic*		pInstance = new CMagic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMagic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMagic::Clone(void * pArg)
{
	CMagic*		pInstance = new CMagic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMagic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMagic::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pParentTransformCom);

}


