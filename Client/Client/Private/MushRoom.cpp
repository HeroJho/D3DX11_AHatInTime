#include "stdafx.h"
#include "..\Public\MushRoom.h"
#include "GameInstance.h"

#include "ToolManager.h"

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
	m_eState = STATE_DOWN_IDLE;


	// 내 상태 정의
	// m_pTransformCom->Set_RealOriScale(XMVectorSet(2.f, 2.f, 2.f, 1.f));
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_Desc.vScale));

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_Desc.vRotation.x
		, XMVectorSet(0.f, 1.f, 0.f, 0.f), m_Desc.vRotation.y
		, XMVectorSet(0.f, 0.f, 1.f, 0.f), m_Desc.vRotation.z);

	_vector vPos = XMLoadFloat3(&m_Desc.vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);



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
			m_pLight->Get_LightEditDesc()->vDiffuse = _float4(0.f, 0.f, 0.f, 1.f);
			m_pLight->Get_LightEditDesc()->vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
			break;
		case STATE_DOWN_START:
			break;
		case STATE_UP_START:
			break;
		case STATE_UP_IDLE:
			m_fRatio = 1.f;
			m_pLight->Get_LightEditDesc()->vDiffuse = m_Desc.vDiffuseColor;
			m_pLight->Get_LightEditDesc()->vAmbient = m_Desc.vAmColor;
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
	// 가만히 있는다
	
}

void CMushRoom::Tick_Down_Start(_float fTimeDelta)
{
	m_fRatio -= m_Desc.fDownSpeed * fTimeDelta;

	if (0.f > m_fRatio)
	{
		m_fRatio = 0.f;

		Set_State(STATE_DOWN_IDLE);

		return;
	}

	_float4 vDiffuColor(m_Desc.vDiffuseColor.x *m_fRatio, m_Desc.vDiffuseColor.y *m_fRatio, m_Desc.vDiffuseColor.z *m_fRatio, 1.f);
	_float4 vAmbColor(m_Desc.vAmColor.x *m_fRatio, m_Desc.vAmColor.y *m_fRatio, m_Desc.vAmColor.z *m_fRatio, 1.f);
	m_pLight->Get_LightEditDesc()->vDiffuse = vDiffuColor;
	m_pLight->Get_LightEditDesc()->vAmbient = vAmbColor;
}

void CMushRoom::Tick_Up_Start(_float fTimeDelta)
{

	m_fRatio += m_Desc.fUpSpeed * fTimeDelta;

	if (1.f < m_fRatio)
	{
		m_fRatio = 1.f;

		Set_State(STATE_UP_IDLE);

		return;
	}

	_float4 vDiffuColor(m_Desc.vDiffuseColor.x *m_fRatio, m_Desc.vDiffuseColor.y *m_fRatio, m_Desc.vDiffuseColor.z *m_fRatio, 1.f);
	_float4 vAmbColor(m_Desc.vAmColor.x *m_fRatio, m_Desc.vAmColor.y *m_fRatio, m_Desc.vAmColor.z *m_fRatio, 1.f);
	m_pLight->Get_LightEditDesc()->vDiffuse = vDiffuColor;
	m_pLight->Get_LightEditDesc()->vAmbient = vAmbColor;

}

void CMushRoom::Tick_Up_Idle(_float fTimeDelta)
{
	// 켜서 가만히 있는다
}






void CMushRoom::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;


	Tick_Col(m_pTransformCom->Get_WorldMatrix(), nullptr, nullptr);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);
	RELEASE_INSTANCE(CGameInstance);


	Set_State(STATE_DOWN_START);
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


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 9)))
			return E_FAIL;
	}


	Render_Col();


	return S_OK;
}

void CMushRoom::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	if ("Tag_Player" == Desc.pOther->Get_Tag() && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
	{
		Set_State(STATE_UP_START);
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
