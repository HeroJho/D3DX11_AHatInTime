#include "stdafx.h"
#include "..\Public\Flask.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "ParticleManager.h"
#include "CamManager.h"

#include "Player.h"
#include "Camera_Free.h"

#include "Flask_EX.h"
#include "FlaskLight.h"
#include "WitchChargEffect.h"
#include "Wind_Spiral.h"

CFlask::CFlask(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CItem(pDevice, pContext)
{

}

CFlask::CFlask(const CFlask & rhs)
	: CItem(rhs)
{

}

HRESULT CFlask::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CFlask::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	ITEMDESC* ItemDesc = (ITEMDESC*)pArg;
	if (nullptr == ItemDesc->pDesc)
		return E_FAIL;
	FLASKDESC* FlaskDesc = (FLASKDESC*)(ItemDesc->pDesc);

	memcpy(&m_Desc, FlaskDesc, sizeof(FLASKDESC));


	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_sTag = "Tag_Item";

	m_pTransformCom->Jump(m_Desc.fJumpPow);







	return S_OK;
}

void CFlask::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	__super::Tick(fTimeDelta);

	if (!m_bOn)
	{
		_vector vDir = XMLoadFloat3(&m_Desc.vDir);
		_vector vRightDir =  XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector3Normalize(vDir));
		m_pTransformCom->Go_Dir(vDir, m_Desc.fDirPow, fTimeDelta, m_pNavigationCom);
		m_pTransformCom->Turn(vRightDir, 10.f, fTimeDelta);


		m_fParticleTimeAcc += fTimeDelta;
		// if (0.1f < m_fParticleTimeAcc)
		{
			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_star_shuriken"), vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.1f, 0.1f, 0.1f), _float3(0.05f, 0.05f, 0.05f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.f, 0.f, false, 0.f, 0.f, 0.5f,
				1, 0.3f, 0.05f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), CParticle::TYPE_TEXTURE);
			m_fParticleTimeAcc = 0.f;
		}
	}

}

void CFlask::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	__super::LateTick(fTimeDelta);

	m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 3.f);


	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(m_pTransformCom->Get_WorldMatrix(), m_pNavigationCom, m_pTransformCom);


	// ÂøÁö Çß´Ù¸é
	_float fTemp = 0.f;
	if (m_pNavigationCom->isGround(m_pTransformCom->Get_State(CTransform::STATE_POSITION), &fTemp) 
		|| COBB::COL_ON == Get_StaticOBB()->Get_ColState()
		|| COBB::COL_BLOCK == Get_StaticOBB()->Get_ColState())
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
			40, 0.f, 0.5f, 0.f, 0.f, 0.f, 0.1f, 0.f, 0.1f, 0.1f, _float3(-90.f, 0.f, -90.f), _float3(90.f, 0.f, 90.f), CParticle::TYPE_MODLE);
		CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), Desc.vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.f, 2.f, false, 0.f, 0.f, 2.f,
			10, 2.f, 0.5f, 0.f, 0.f, 0.f, 0.1f, 0.f, 0.1f, 0.1f, _float3(-10.f, 0.f, -10.f), _float3(10.f, 0.f, 10.f), CParticle::TYPE_MODLE);
		CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), Desc.vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(90.f, 0.f, 0.f), 0.1f, 4.f, false, 0.f, 0.f, 2.f,
			20, 0.f, 0.5f, 0.f, 0.f, 0.f, 0.1f, 0.f, 0.1f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(0.f, 360.f, 0.f), CParticle::TYPE_MODLE);

	/*	CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_T_FX_Flare_01"), Desc.vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(1.5f, 1.5f, 1.5f), _float3(0.5f, 0.5f, 0.5f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.05f, 3.f, true, 0.1f, 2.f, 2.f,
			20, 2.f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, _float3(-90.f, 0.f, -90.f), _float3(90.f, 0.f, 90.f), CParticle::TYPE_TEXTURE);

*/
		CCamManager::Get_Instance()->Get_Cam()->Start_Shake(0.2f, 10.f, 0.07f);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		LEVEL eLevel = CToolManager::Get_Instance()->Get_CulLevel();

		//CFlaskLight::FLASKLIGHTDESC LightDesc;
		//XMStoreFloat3(&LightDesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		//pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_FlaskLight"), eLevel, TEXT("Layer_Light"), &LightDesc);

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

		
		
		RELEASE_INSTANCE(CGameInstance);

		

		Set_Dead(true);
	}



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_PROJECT, this);
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CFlask::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();

	return S_OK;
}

HRESULT CFlask::SetUp_State(_fmatrix StateMatrix)
{
	if (FAILED(__super::SetUp_State(StateMatrix)))
		return E_FAIL;

	return S_OK;
}


void CFlask::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if("Tag_MushRoom" != Desc.pOther->Get_Tag() && !strcmp("Attacked_Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
		m_bEx = true;
}






void CFlask::Use_Item()
{



}






HRESULT CFlask::Ready_Components()
{

	LEVEL eLevel = CToolManager::Get_Instance()->Get_CulLevel();

	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = m_Desc.iNaviIndex;
	if (FAILED(__super::Add_Component(eLevel, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
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

CFlask * CFlask::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFlask*		pInstance = new CFlask(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFlask"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlask::Clone(void * pArg)
{
	CFlask*		pInstance = new CFlask(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFlask"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlask::Free()
{
	__super::Free();


	Safe_Release(m_pNavigationCom);

}


