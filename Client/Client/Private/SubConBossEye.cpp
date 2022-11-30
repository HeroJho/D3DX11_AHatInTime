#include "stdafx.h"
#include "..\Public\SubconBossEye.h"
#include "GameInstance.h"

#include  "ToolManager.h"
#include "GameManager.h"
#include "CutSceneManager.h"

#include "Player.h"


CSubconBossEye::CSubconBossEye(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CSubconBossEye::CSubconBossEye(const CSubconBossEye & rhs)
	: CMonster(rhs)
{
}






HRESULT CSubconBossEye::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSubconBossEye::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Monster";


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_CreatureDesc.vPos), 1.f));
	m_pTransformCom->Set_CurSpeed(1.f);
	// m_pTransformCom->Set_RealOriScale(XMVectorSet(1.f, 1.f, 1.f, 1.f));


	Equip_Sockat("Ori_Hat", SLOT_HEAD);
	Equip_Sockat("Umbrella", SLOT_SPIN);

	return S_OK;
}




void CSubconBossEye::Attacked(_int iAT)
{
	m_pTransformCom->ReSet_AttackedAnim();
	m_eState = STATE_ATTACKED;
}

void CSubconBossEye::Dead_Tick(_float fTimeDelta)
{
	CCutSceneManager::Get_Instance()->Add_Obj(this, 0);

	if (!m_bStartDisApear)
		return;



	m_fAlpa -= fTimeDelta;

	if (0.f > m_fAlpa)
	{
		m_fAlpa = 0.f;
		Set_Dead(true);
	}

}






void CSubconBossEye::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);



	switch (m_eState)
	{
	case STATE_IDLE:
	{

	}
	break;
	case STATE_ATTACKED:
	{
		if (m_pTransformCom->Tick_AttackAnim(fTimeDelta))
			m_eState = STATE_IDLE;
	}
	break;
	case STATE_DEAD:
	{
		Dead_Tick(fTimeDelta);
	}
	break;
	default:
		break;
	}




}


void CSubconBossEye::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);

	if (nullptr == m_pRendererCom)
		return;




	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld);



	m_pModelCom->Play_Animation(fTimeDelta);


	// 소켓 갱신
	m_pSockatCom->Tick(fTimeDelta, m_pTransformCom);


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	

	if (pGameInstance->Key_Down(DIK_M))
	{
		m_bDrw = true;
	}

	if (m_bDrw)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
		m_pSockatCom->LateTick(fTimeDelta, m_pRendererCom, CRenderer::RENDER_NONLIGHT);
	}


	
	RELEASE_INSTANCE(CGameInstance);

}

HRESULT CSubconBossEye::Render()
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


	_bool bBlur = false;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bBlur", &bBlur, sizeof(_bool))))
		return E_FAIL;



	_uint iPassIndex = 5;
	if (STATE_DEAD == m_eState)
	{
		iPassIndex = 3;
		if (FAILED(m_pShaderCom->Set_RawValue("g_AlpaValue", &m_fAlpa, sizeof(_float))))
			return E_FAIL;
	}


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}


	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();


	return S_OK;
}

void CSubconBossEye::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("ChaseSphere", Desc.MyDesc.sTag))
		{
			// if (CGameManager::Get_Instance()->Check_Stage_1())
			{
				CCutSceneManager::Get_Instance()->StartCutScene(CCutSceneManager::CUT_EYE);
				m_eState = STATE_DEAD;
			}

		}
	}
}














HRESULT CSubconBossEye::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("SubconEye"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	if (FAILED(Ready_Sockat()))
		return E_FAIL;


	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));


	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(.5f, .5f, .5f);
	strcpy_s(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(10.f, 10.f, 10.f);
	strcpy_s(ColDesc.sTag, "ChaseSphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	return S_OK;
}





HRESULT CSubconBossEye::Ready_Sockat()
{
	/* For.Com_Sockat */
	CSockat::SOCATDESC SockatDesc;
	ZeroMemory(&SockatDesc, sizeof(CSockat::SOCATDESC));
	XMStoreFloat4x4(&SockatDesc.mPivot, m_pModelCom->Get_PivotMatrix());
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sockat"), TEXT("Com_Sockat"), (CComponent**)&m_pSockatCom, &SockatDesc)))
		return E_FAIL;


	if (FAILED(m_pSockatCom->Match_Enum_BoneName("skull", SLOT_HEAD)))
		return E_FAIL;
	if (FAILED(m_pSockatCom->Match_Enum_BoneName("SubconEye01", SLOT_SPIN)))
		return E_FAIL;



	return S_OK;
}




HRESULT CSubconBossEye::Equip_Sockat(string sItemName, SLOT eSlot)
{

	// 중복으로 착용하려고 그럼 나간다
	if (m_pSockatCom->Check_IsHaveSocket(eSlot, sItemName))
		return S_OK;

	// Item을 착용 중이냐
	// 그럼 기존거를 삭제하고 끼운다
	if (!m_pSockatCom->Check_Sockat(eSlot))
		m_pSockatCom->Remove_Sockat(eSlot);


	char cItemNameTemp[MAX_PATH];
	strcpy_s(cItemNameTemp, sItemName.data());
	TCHAR cItemName[MAX_PATH];
	CToolManager::Get_Instance()->CtoTC(cItemNameTemp, cItemName);


	CSockat::PARTSDESC PartsDesc;
	ZeroMemory(&PartsDesc, sizeof(CSockat::PARTSDESC));


	if (!lstrcmp(cItemName, TEXT("Ori_Hat")))
	{
		lstrcpy(PartsDesc.m_szModelName, TEXT("Ori_Hat"));
		PartsDesc.vPos = _float3(0.02f, 0.f, -1.2f);
		PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
		PartsDesc.vRot = _float3(-90.f, -16.2f, 0.f);
		PartsDesc.pOwner = this;
	}
	else if (!lstrcmp(cItemName, TEXT("Umbrella")))
	{
		lstrcpy(PartsDesc.m_szModelName, TEXT("Umbrella"));
		PartsDesc.vPos = _float3(0.11f, 0.f, -0.64f);
		PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
		PartsDesc.vRot = _float3(90.f, 27.1f, 0.f);
		PartsDesc.pOwner = this;
	}


	if (!FAILED(m_pSockatCom->Add_Sockat(eSlot, m_pModelCom, TEXT("Prototype_GameObject_Parts"), PartsDesc)))
		return E_FAIL;

	return S_OK;
}






CSubconBossEye * CSubconBossEye::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSubconBossEye*		pInstance = new CSubconBossEye(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSubSpider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSubconBossEye::Clone(void * pArg)
{
	CSubconBossEye*		pInstance = new CSubconBossEye(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSubSpider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSubconBossEye::Free()
{
	__super::Free();

	Safe_Release(m_pSockatCom);
}
