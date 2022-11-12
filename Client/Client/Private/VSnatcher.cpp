#include "stdafx.h"
#include "..\Public\VSnatcher.h"
#include "GameInstance.h"

#include  "ToolManager.h"
#include "DataManager.h"

#include "Player.h"

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


	// Set_State(STATE_IDLE);
	

	m_iIndexs.push_back(9);
	m_iIndexs.push_back(18);

	m_iIndexs.push_back(24);
	m_iIndexs.push_back(21);

	m_iIndexs.push_back(6);
	m_iIndexs.push_back(7);

	m_iIndexs.push_back(11);
	
	
	m_iIndexs.push_back(25);
	m_iIndexs.push_back(26);
	m_iIndexs.push_back(0);


	m_pModelCom->Set_AnimIndex(m_iIndexs[m_iIndex]);

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
		/*switch (m_eState)
		{
		case Client::CSubSpider::MONSTER_ATTACKED:
		break;
		}*/
	}

	Set_Anim();
}

void CVSnatcher::Set_Anim()
{

	switch (m_eState)
	{
	case STATE_APPEAR:
		break;
	case STATE_SOFTAPPEAR:
		break;
	case STATE_DISAPPEAR:
		break;
	case STATE_CURSE:
		break;
	case STATE_CURSELOOP:
		break;
	case STATE_HANDHOLD:
		break;
	case STATE_STEALHAT:
		break;
	case STATE_TALKIDLE:
		break;
	case STATE_TALKING:
		break;
	case STATE_THINKING:
		break;
	case STATE_END:
		break;
	default:
		break;
	}

}



void CVSnatcher::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_N))
	{
		m_iIndex++;
		if (9 < m_iIndex)
			m_iIndex = 0;
	}
	else if (pGameInstance->Key_Down(DIK_B))
	{
		m_iIndex = 0;
		Set_Dead(true);
	}

	RELEASE_INSTANCE(CGameInstance);

	switch (m_eState)
	{


	}

}



void CVSnatcher::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);

	if (nullptr == m_pRendererCom)
		return;

	m_pModelCom->Set_AnimIndex(m_iIndexs[m_iIndex]);

	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld);



	if (m_pModelCom->Play_Animation(fTimeDelta))
	{

	}



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);

	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	if (true == isDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

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
	ColDesc.vSize = _float3(.5f, .5f, .5f);
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

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
