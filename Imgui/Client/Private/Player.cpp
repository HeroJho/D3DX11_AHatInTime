#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_eState = STATE_IDLE;

	m_fWalkSpeed = 1.5f;
	m_fRunSpeed = 2.5f;
	m_fTurnSpeed = 0.6f;
	m_fRotationSpeed = 3.5f;

	ANIM_LINEAR_DATA LinearData;
	ZeroMemory(&LinearData, sizeof(ANIM_LINEAR_DATA));
	LinearData.fLimitRatio = 0.05f;
	LinearData.fTickPerSeconed = 0.4f;
	LinearData.iMyIndex = 111;
	LinearData.iTargetIndex = 197;

	m_pModelCom->Push_AnimLinearData(LinearData);
	
	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	m_TickStates.push_back(STATE_IDLE);

	Input(fTimeDelta);

}
void CPlayer::Input(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (pGameInstance->Key_Pressing(DIK_W))
	{

		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_pTransformCom->Go_Straight(m_fRunSpeed, fTimeDelta);
			m_TickStates.push_back(STATE_RUN);
		}
		else
		{
			m_pTransformCom->Go_Straight(m_fWalkSpeed, fTimeDelta);
			m_TickStates.push_back(STATE_WALK);
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{

	}

	if (pGameInstance->Key_Pressing(DIK_A))
	{
		if (!pGameInstance->Key_Pressing(DIK_W))
			m_pTransformCom->Go_Straight(m_fTurnSpeed, fTimeDelta);

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -m_fRotationSpeed, fTimeDelta);
		m_TickStates.push_back(STATE_WALK);
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		if (!pGameInstance->Key_Pressing(DIK_W))
			m_pTransformCom->Go_Straight(m_fTurnSpeed, fTimeDelta);

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRotationSpeed, fTimeDelta);
		m_TickStates.push_back(STATE_WALK);
	}




	RELEASE_INSTANCE(CGameInstance);
}


void CPlayer::Set_State()
{

	list<STATE>::iterator iter = max_element(m_TickStates.begin(), m_TickStates.end());

	m_eState = (*iter);

	Set_Anim();
	m_TickStates.clear();
}

void CPlayer::Set_Anim()
{
	switch (m_eState)
	{
	case Client::CPlayer::STATE_IDLE:
		m_pModelCom->Set_AnimIndex(111);
		break;
	case Client::CPlayer::STATE_WALK:
		m_pModelCom->Set_AnimIndex(197);
		break;
	case Client::CPlayer::STATE_RUN:
		m_pModelCom->Set_AnimIndex(198);
		break;
	}
}





void CPlayer::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	Set_State();
}

HRESULT CPlayer::Render()
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


HRESULT CPlayer::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, TEXT("HatGirl"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
