#include "stdafx.h"
#include "..\Public\ExPlo.h"
#include "GameInstance.h"

#include "IceBox.h"
#include "Player.h"
#include "ParticleManager.h"
#include "CamManager.h"
#include "Camera_Free.h"
#include "ToolManager.h"
#include "Wind_Spiral.h"
#include "PuzzleCube_Boss.h"

CExPlo::CExPlo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CExPlo::CExPlo(const CExPlo& rhs)
	: CGameObject(rhs)
{
}

HRESULT CExPlo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CExPlo::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;




	if (FAILED(Ready_Components()))
		return E_FAIL;

	EXPLODESC* Desc = (EXPLODESC*)pArg;


	Desc->vPos.y = 0.2f;
	_vector vPos = XMLoadFloat3(&Desc->vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), Desc->vPos, _float3(0.f, 3.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(3.f, 3.f, 3.f), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 4.f, 15.f, false, 0.f, 0.f, 2.f,
		40, 0.f, 0.5f, 0.f, 0.f, 0.f, 0.1f, 0.f, 0.1f, 0.1f, _float3(-90.f, 0.f, -90.f), _float3(90.f, 0.f, 90.f), CParticle::TYPE_MODLE);
	CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), Desc->vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.f, 2.f, false, 0.f, 0.f, 2.f,
		10, 2.f, 0.5f, 0.f, 0.f, 0.f, 0.1f, 0.f, 0.1f, 0.1f, _float3(-10.f, 0.f, -10.f), _float3(10.f, 0.f, 10.f), CParticle::TYPE_MODLE);
	CParticleManager::Get_Instance()->Create_Effect(TEXT("SmokeParticle"), Desc->vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(90.f, 0.f, 0.f), 0.1f, 4.f, false, 0.f, 0.f, 2.f,
		20, 0.f, 0.5f, 0.f, 0.f, 0.f, 0.1f, 0.f, 0.1f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(0.f, 360.f, 0.f), CParticle::TYPE_MODLE);
	CParticleManager::Get_Instance()->Create_Effect(TEXT("Grave1"), Desc->vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f), _float3(2.f, 2.f, 2.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 90.f), 0.5f, 15.f, true, 1.f, 3.f, 0.8f,
		20, 2.f, 0.5f, 0.f, 0.f, 0.f, 5.f, 0.f, 1.0f, 0.1f, _float3(0.f, 0.f, 0.f), _float3(0.f, 360.f, 0.f), CParticle::TYPE_MODLE);

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

	pGameInstance->PlaySoundW(L"bombcake_explode.ogg", SOUND_MEFFECT, g_fEffectSound + 0.2f);

	RELEASE_INSTANCE(CGameInstance);


	CCamManager::Get_Instance()->Get_Cam()->Start_Shake(0.2f, 10.f, 0.07f);






	return S_OK;
}

void CExPlo::Tick(_float fTimeDelta)
{
	if (m_bTickAttack)
		Set_Dead(true);
}

void CExPlo::LateTick(_float fTimeDelta)
{
	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_PROJECT, this);
	RELEASE_INSTANCE(CGameInstance);

	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	m_bTickAttack = true;
}

HRESULT CExPlo::Render()
{


	Render_Col();

	return S_OK;
}



HRESULT CExPlo::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, , TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	strcpy(ColDesc.sTag, "EX_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	return S_OK;
}



void CExPlo::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if (!strcmp("EX_Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
	{
		if ("Tag_Player" == Desc.pOther->Get_Tag())
		{
			CPlayer* pPlayer = (CPlayer*)Desc.pOther;
			pPlayer->Attacked();
		}
		else if ("Tag_PuzzleCube_Boss" == Desc.pOther->Get_Tag())
		{
			CPuzzleCube_Boss* pPuzzleCube = (CPuzzleCube_Boss*)Desc.pOther;
			pPuzzleCube->Attacked();
		}


		m_bTickAttack = true;
	}


}



CExPlo * CExPlo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CExPlo*		pInstance = new CExPlo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CExPlo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CExPlo::Clone(void * pArg)
{
	CExPlo*		pInstance = new CExPlo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CExPlo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CExPlo::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
