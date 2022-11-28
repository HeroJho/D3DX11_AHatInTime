#include "stdafx.h"
#include "..\Public\MushRoom.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "GameManager.h"

#include "Light.h"

CMushRoom::CMushRoom(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMushRoom::CMushRoom(const CMushRoom & rhs)
	: CGameObject(rhs)
{
}






HRESULT CMushRoom::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMushRoom::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	memcpy(&m_Desc, pArg, sizeof(MUSHROOMDESC));


	m_sTag = "Tag_MushRoom";
	Set_State(STATE_DOWN_START);


	// 내 상태 정의
	// m_pTransformCom->Set_RealOriScale(XMVectorSet(2.f, 2.f, 2.f, 1.f));
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_Desc.vScale));

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_Desc.vRotation.x
		, XMVectorSet(0.f, 1.f, 0.f, 0.f), m_Desc.vRotation.y
		, XMVectorSet(0.f, 0.f, 1.f, 0.f), m_Desc.vRotation.z);

	_vector vPos = XMLoadFloat3(&m_Desc.vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	if (!m_Desc.iIsLight)
	{
		// 빛 정의
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		LIGHTDESC LightDesc;
		LightDesc.eType = LIGHTDESC::TYPE_POINT;
		LightDesc.vDiffuse = m_Desc.vDiffuseColor;
		LightDesc.vAmbient = m_Desc.vAmColor;

		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 2.5f);
		XMStoreFloat4(&LightDesc.vPosition, XMVectorSetW(vPos, 1.f));

		LightDesc.fRange = m_Desc.fPower;

		pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, &m_pLight);

		RELEASE_INSTANCE(CGameInstance);
	}




	return S_OK;
}



void CMushRoom::Set_State(STATE eState)
{
	if (eState != m_eState)
	{
		switch (m_eState)
		{
		case STATE_DOWN_IDLE:
			m_fRatio = 0.f;
			if (nullptr != m_pLight)
			{
				m_pLight->Get_LightEditDesc()->vDiffuse = _float4(0.f, 0.f, 0.f, 1.f);
				m_pLight->Get_LightEditDesc()->vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
			}

			break;
		case STATE_DOWN_START:
			m_fTimeAcc = 0.f;
			m_fTime = CToolManager::Get_Instance()->Get_RendomNum(0.5f, 5.f);
			break;
		case STATE_UP_START:
			m_fTimeAcc = 0.f;
			m_fTime = CToolManager::Get_Instance()->Get_RendomNum(0.5f, 5.f);
			break;
		case STATE_UP_IDLE:
			m_fRatio = 1.f;
			if (nullptr != m_pLight)
			{
				m_pLight->Get_LightEditDesc()->vDiffuse = m_Desc.vDiffuseColor;
				m_pLight->Get_LightEditDesc()->vAmbient = m_Desc.vAmColor;
			}
			break;
		default:
			break;
		}
	}

	m_eState = eState;

}






void CMushRoom::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);


	if (CGameManager::Get_Instance()->Get_Musroom())
	{
		if (STATE_UP_IDLE != m_eState && STATE_UP_START != m_eState)
			Set_State(STATE_UP_START);
	}


	switch (m_eState)
	{
	case Client::CMushRoom::STATE_DOWN_IDLE:
		Tick_Down_Idle(fTimeDelta);
		break;
	case Client::CMushRoom::STATE_DOWN_START:
		Tick_Down_Start(fTimeDelta);
		break;
	case Client::CMushRoom::STATE_UP_START:
		Tick_Up_Start(fTimeDelta);
		break;
	case Client::CMushRoom::STATE_UP_IDLE:
		Tick_Up_Idle(fTimeDelta);
		break;
	default:
		break;
	}

}

void CMushRoom::Tick_Down_Idle(_float fTimeDelta)
{
	if (nullptr == m_pLight)
		return;

	//  가만히 있는다
	_float4 vDiffuColor(m_Desc.vDiffuseColor.x *m_fRatio, m_Desc.vDiffuseColor.y *m_fRatio, m_Desc.vDiffuseColor.z *m_fRatio, 1.f);
	_float4 vAmbColor(m_Desc.vAmColor.x *m_fRatio, m_Desc.vAmColor.y *m_fRatio, m_Desc.vAmColor.z *m_fRatio, 1.f);
	m_pLight->Get_LightEditDesc()->vDiffuse = vDiffuColor;
	m_pLight->Get_LightEditDesc()->vAmbient = vAmbColor;


}

void CMushRoom::Tick_Down_Start(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	if (m_fTime > m_fTimeAcc)
		return;

	m_fRatio -= m_Desc.fDownSpeed * fTimeDelta;

	if (0.f > m_fRatio)
	{
		m_fRatio = 0.f;

		Set_State(STATE_DOWN_IDLE);

		return;
	}

	if (m_pLight)
	{
		_float4 vDiffuColor(m_Desc.vDiffuseColor.x *m_fRatio, m_Desc.vDiffuseColor.y *m_fRatio, m_Desc.vDiffuseColor.z *m_fRatio, 1.f);
		_float4 vAmbColor(m_Desc.vAmColor.x *m_fRatio, m_Desc.vAmColor.y *m_fRatio, m_Desc.vAmColor.z *m_fRatio, 1.f);
		m_pLight->Get_LightEditDesc()->vDiffuse = vDiffuColor;
		m_pLight->Get_LightEditDesc()->vAmbient = vAmbColor;
	}

}

void CMushRoom::Tick_Up_Start(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	if (m_fTime > m_fTimeAcc)
		return;

	m_fRatio += m_Desc.fUpSpeed * fTimeDelta;

	if (1.f < m_fRatio)
	{
		m_fRatio = 1.f;

		Set_State(STATE_UP_IDLE);

		return;
	}

	if (m_pLight)
	{
		_float4 vDiffuColor(m_Desc.vDiffuseColor.x *m_fRatio, m_Desc.vDiffuseColor.y *m_fRatio, m_Desc.vDiffuseColor.z *m_fRatio, 1.f);
		_float4 vAmbColor(m_Desc.vAmColor.x *m_fRatio, m_Desc.vAmColor.y *m_fRatio, m_Desc.vAmColor.z *m_fRatio, 1.f);
		m_pLight->Get_LightEditDesc()->vDiffuse = vDiffuColor;
		m_pLight->Get_LightEditDesc()->vAmbient = vAmbColor;
	}
}

void CMushRoom::Tick_Up_Idle(_float fTimeDelta)
{
	// 켜서 가만히 있는다

	if (0.8f < m_fRatio)
	{
		m_bTurn = true;
	}
	else if (0.7f > m_fRatio)
	{
		m_bTurn = false;
	}

	if (m_bTurn)
	{
		m_fRatio -= m_Desc.fUpSpeed * fTimeDelta;
	}
	else
	{
		m_fRatio += m_Desc.fUpSpeed * fTimeDelta;
	}

	if (m_pLight)
	{
		_float4 vDiffuColor(m_Desc.vDiffuseColor.x *m_fRatio, m_Desc.vDiffuseColor.y *m_fRatio, m_Desc.vDiffuseColor.z *m_fRatio, 1.f);
		_float4 vAmbColor(m_Desc.vAmColor.x *m_fRatio, m_Desc.vAmColor.y *m_fRatio, m_Desc.vAmColor.z *m_fRatio, 1.f);
		m_pLight->Get_LightEditDesc()->vDiffuse = vDiffuColor;
		m_pLight->Get_LightEditDesc()->vAmbient = vAmbColor;
	}


	if (!CGameManager::Get_Instance()->Get_Musroom())
		Set_State(STATE_DOWN_START);
}


HRESULT CMushRoom::Render_ShadowDepth()
{

	if (nullptr == m_pShaderCom ||
		nullptr == m_pTransformCom)
		return E_FAIL;



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;


	_vector vPos = XMVectorSetW(XMLoadFloat3(&pGameInstance->Get_PlayerPos()), 1.f);
	_vector		vLightAt = XMVectorSetW(vPos, 1.f);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 5.f);
	vPos = XMVectorSetX(vPos, XMVectorGetX(vPos) - 7.f);
	_vector		vLightEye = XMVectorSetW(vPos, 1.f);
	_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_float4x4		LightViewMatrix;
	XMStoreFloat4x4(&LightViewMatrix, XMMatrixTranspose(XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp)));
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &LightViewMatrix, sizeof(_float4x4))))
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




void CMushRoom::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;


	Tick_Col(m_pTransformCom->Get_WorldMatrix(), nullptr, nullptr);



	if (!CGameManager::Get_Instance()->Check_IsInWisp(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	{
		// m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);

		if (STATE_UP_START == m_eState || STATE_UP_IDLE == m_eState || STATE_DOWN_START == m_eState)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
		else
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_EM, this);
	RELEASE_INSTANCE(CGameInstance);


	CGameManager::Get_Instance()->Set_Musroom(false);

	//Set_State(STATE_UP_START);
	//Set_State(STATE_DOWN_START);
}



HRESULT CMushRoom::Render()
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

	if (FAILED(m_pShaderCom->Set_RawValue("vMushColor", &m_Desc.vDiffuseColor, sizeof(_float4))))
		return E_FAIL;

	_bool bBlur = false;

	_uint iIndex = 7;
	if (STATE_UP_START == m_eState || STATE_UP_IDLE == m_eState || STATE_DOWN_START == m_eState)
	{
		if (STATE_DOWN_START == m_eState)
			bBlur = false;
		else
			bBlur = true;
		iIndex = 9;
		if (FAILED(m_pShaderCom->Set_RawValue("g_MushColorRatio", &m_fRatio, sizeof(_float))))
			return E_FAIL;
	}

	// bBlur = true;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bBlur", &bBlur, sizeof(_bool))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iIndex)))
			return E_FAIL;
	}


	// Render_Col();

	return S_OK;
}

void CMushRoom::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	if ("Tag_Player" == Desc.pOther->Get_Tag() && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
	{
		CGameManager::Get_Instance()->Set_Musroom(true);
		// Set_State(STATE_UP_START);
	}


}


















HRESULT CMushRoom::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Mushroom"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(10.f, 10.f, 10.f);
	strcpy(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;





	return S_OK;
}










CMushRoom * CMushRoom::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMushRoom*		pInstance = new CMushRoom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMushRoom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMushRoom::Clone(void * pArg)
{
	CMushRoom*		pInstance = new CMushRoom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMushRoom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMushRoom::Free()
{
	__super::Free();



	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
