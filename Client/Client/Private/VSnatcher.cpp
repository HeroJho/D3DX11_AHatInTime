#include "stdafx.h"
#include "..\Public\VSnatcher.h"
#include "GameInstance.h"

#include  "ToolManager.h"
#include "DataManager.h"
#include "ToolManager.h"
#include "CamManager.h"

#include "Player.h"
#include "Camera_Free.h"
#include "ExPlo.h"

CVSnatcher::CVSnatcher(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CVSnatcher::CVSnatcher(const CVSnatcher & rhs)
	: CGameObject(rhs)
{
}






HRESULT CVSnatcher::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVSnatcher::Initialize(void * pArg)
{
	// __super::Initialize(pArg);

 	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Snatcher";

	list<ANIM_LINEAR_DATA> LinearDatas = CDataManager::Get_Instance()->Load_Anim("VSnatcher");
	for (auto& Data : LinearDatas)
		Set_AnimLinearData(Data);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-60.57f, 0.101f, -115.45f, 1.f));
	m_pTransformCom->Set_CurSpeed(1.f);

	m_pTransformCom->Set_RealOriScale(XMVectorSet(2.f, 2.f, 2.f, 1.f));


	 Set_State(STATE_DISAPPEAR);
	
	 CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	 m_pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr(LEVEL_STATIC, TEXT("Layer_Player"), 0);
	 Safe_AddRef(m_pPlayer);
	 RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}







void CVSnatcher::Set_AnimLinearData(ANIM_LINEAR_DATA LinearData)
{
	m_pModelCom->Push_AnimLinearData(LinearData);
}

void CVSnatcher::Set_State(STATE eState)
{
	m_ePreState = m_eState;
	m_eState = eState;


	if (m_ePreState != m_eState)
	{
		switch (m_eState)
		{
		case STATE_APPEAR:
			break;
		case STATE_SOFTAPPEAR:
			break;
		case STATE_DISAPPEAR:
			break;
		case STATE_IDLE:
			break;
		case STATE_TALKING:
			break;
		case STATE_CURSESTART:
			m_fCurseTimeAcc = 0.f;
			m_iCurseCount = 5;
			break;
		case STATE_CURSE:
			break;
		case STATE_MINON:
			m_fMinonTimeAcc = 0.f;
			break;
		default:
			break;
		}
	}

	Set_Anim();
}

void CVSnatcher::Set_Anim()
{

	switch (m_eState)
	{
	case STATE_APPEAR:
		m_pModelCom->Set_AnimIndex(0);
		break;
	case STATE_SOFTAPPEAR:
		m_pModelCom->Set_AnimIndex(18);
		break;
	case STATE_DISAPPEAR:
		m_pModelCom->Set_AnimIndex(9);
		break;
	case STATE_IDLE:
		m_pModelCom->Set_AnimIndex(24);
		break;
	case STATE_TALKING:
		m_pModelCom->Set_AnimIndex(25);
		break;
	case STATE_CURSESTART:
		m_pModelCom->Set_AnimIndex(6);
		break;
	case STATE_CURSE:
		m_pModelCom->Set_AnimIndex(7);
		break;
	case STATE_MINON:
		m_pModelCom->Set_AnimIndex(11);
	default:
		break;
	}


}


void CVSnatcher::Compute_Pattern(_float fTimeDelta)
{
	_uint iRendNum = CToolManager::Get_Instance()->Get_RendomNum_Int(0, 1);

	switch (iRendNum)
	{
	case 0:
		Set_State(STATE_CURSESTART);
		break;
	case 1:
		Set_State(STATE_MINON);
		break;
	default:
		break;
	}

}

void CVSnatcher::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);


	switch (m_eState)
	{
	case STATE_APPEAR:
		Tick_Appear(fTimeDelta);
		break;
	case STATE_SOFTAPPEAR:
		Tick_SoftAppear(fTimeDelta);
		break;
	case STATE_DISAPPEAR:
		Tick_DisAppear(fTimeDelta);
		break;
	case STATE_IDLE:
		Tick_Idle(fTimeDelta);
		break;
	case STATE_TALKING:
		Tick_Talking(fTimeDelta);
		break;
	case STATE_CURSESTART:
		Tick_CurseStart(fTimeDelta);
		break;
	case STATE_CURSE:
		Tick_Curse(fTimeDelta);
		break;
	case STATE_MINON:
		Tick_Minon(fTimeDelta);
		break;
	default:
		break;
	}


}

void CVSnatcher::Tick_Appear(_float fTimeDelta)
{
}

void CVSnatcher::Tick_SoftAppear(_float fTimeDelta)
{
}

void CVSnatcher::Tick_DisAppear(_float fTimeDelta)
{

	CTransform* pTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
	
	_vector vPlayerPos = pTran->Get_State(CTransform::STATE_POSITION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDir  = vPlayerPos - vMyPos;
	_vector vNorDir = XMVector3Normalize(vDir);
	_float fDis = XMVectorGetX(XMVector3Length(vDir));

	if (13.f > fDis)
	{
		CCamManager::Get_Instance()->Get_Cam()->Set_Target(this);
		CCamManager::Get_Instance()->Get_Cam()->Set_State(CCamera_Free::CAM_BOSS);
		Set_State(STATE_SOFTAPPEAR);
	}


}

void CVSnatcher::Tick_Idle(_float fTimeDelta)
{

	Compute_Pattern(fTimeDelta);

}

void CVSnatcher::Tick_Talking(_float fTimeDelta)
{
}

void CVSnatcher::Tick_CurseStart(_float fTimeDelta)
{
}

void CVSnatcher::Tick_Curse(_float fTimeDelta)
{
	m_fCurseTimeAcc += fTimeDelta;

	if (0 > m_iCurseCount)
	{
		Set_State(STATE_IDLE);
		m_fCurseTimeAcc = 0.f;
		m_iCurseCount = 0;
		return;
	}

	_vector vPlayerPos = ((CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

	if (2.f < m_fCurseTimeAcc)
	{
		Create_ExPlo(vPlayerPos);
		m_fCurseTimeAcc = 0.f;
		--m_iCurseCount;
	}

	m_pTransformCom->LookAt_ForLandObject(vPlayerPos);
}

void CVSnatcher::Tick_Minon(_float fTimeDelta)
{
	m_fMinonTimeAcc += fTimeDelta;

	if (5.f < m_fMinonTimeAcc)
	{
		Set_State(STATE_IDLE);
		m_fMinonTimeAcc = 0.f;
	}

}







void CVSnatcher::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);

	if (nullptr == m_pRendererCom)
		return;


	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld);



	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		End_Anim();
	}



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);

	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	if (true == isDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	RELEASE_INSTANCE(CGameInstance);
}

void CVSnatcher::End_Anim()
{
	switch (m_eState)
	{
	case STATE_APPEAR:
		break;
	case STATE_SOFTAPPEAR:
		Set_State(STATE_IDLE);
		break;
	case STATE_DISAPPEAR:
		break;
	case STATE_IDLE:
		break;
	case STATE_TALKING:
		break;
	case STATE_CURSESTART:
		Set_State(STATE_CURSE);
		break;
	case STATE_CURSE:
		break;
	}
}







void CVSnatcher::Create_ExPlo(_fvector vPos)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float3 vTempPos;
	XMStoreFloat3(&vTempPos, vPos);

	CExPlo::EXPLODESC Desc;
	Desc.vPos = vTempPos;

	pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_ExPlo"), LEVEL_BOSS, TEXT("Layer_Ex"), &Desc);

	RELEASE_INSTANCE(CGameInstance);
}







HRESULT CVSnatcher::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}


	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();


	return S_OK;
}

void CVSnatcher::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

	}
}














HRESULT CVSnatcher::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("VSnatcher"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));


	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	strcpy(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}












CVSnatcher * CVSnatcher::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVSnatcher*		pInstance = new CVSnatcher(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVSnatcher"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CVSnatcher::Clone(void * pArg)
{
	CVSnatcher*		pInstance = new CVSnatcher(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVSnatcher"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVSnatcher::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
