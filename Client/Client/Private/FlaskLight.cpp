#include "stdafx.h"
#include "..\Public\FlaskLight.h"
#include "GameInstance.h"

#include "IceBox.h"
#include "StatuePosed_Boss.h"
#include "PuzzleCube_Boss.h"
#include "Light.h"


CFlaskLight::CFlaskLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CFlaskLight::CFlaskLight(const CFlaskLight& rhs)
	: CGameObject(rhs)
{
}

HRESULT CFlaskLight::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFlaskLight::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;




	if (FAILED(Ready_Components()))
		return E_FAIL;

	FLASKLIGHTDESC* Desc = (FLASKLIGHTDESC*)pArg;

	_vector vPos = XMLoadFloat3(&Desc->vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vDiffuse = _float4(.529f, .121f, .807f, 1.f);

	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 2.5f);
	XMStoreFloat4(&LightDesc.vPosition, XMVectorSetW(vPos, 1.f));
	LightDesc.vAmbient = _float4(.529f, .121f, .807f, 1.f);
	LightDesc.fRange = 15.f;

	pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, &m_pLight);


	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vDiffuse = _float4(1.f, 165.f/255.f, 0.f, 1.f);

	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) - 1.5f);
	XMStoreFloat4(&LightDesc.vPosition, XMVectorSetW(vPos, 1.f));
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.fRange = 20.f;

	pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, &m_pLight2);
	RELEASE_INSTANCE(CGameInstance);


	

	return S_OK;
}

void CFlaskLight::Tick(_float fTimeDelta)
{
	// m_fTimeAcc += fTimeDelta;
	LIGHTDESC* pDesc = m_pLight->Get_LightEditDesc();
	if (0.f > pDesc->vDiffuse.x)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Remove_Light(m_pLight);
		pGameInstance->Remove_Light(m_pLight2);
		m_pLight = nullptr;
		m_pLight2 = nullptr;
		RELEASE_INSTANCE(CGameInstance);
		Set_Dead(true);
		return;
	}
	else
	{
		pDesc->vDiffuse.x -= m_fLightPower * fTimeDelta;
		pDesc->vDiffuse.y -= m_fLightPower * fTimeDelta;
		pDesc->vDiffuse.z -= m_fLightPower * fTimeDelta;

		pDesc->vAmbient.x -= m_fLightPower * fTimeDelta;
		pDesc->vAmbient.y -= m_fLightPower * fTimeDelta;
		pDesc->vAmbient.z -= m_fLightPower * fTimeDelta;
	}


	pDesc = m_pLight2->Get_LightEditDesc();
	if (0.f > pDesc->vDiffuse.x)
	{
		pDesc->vDiffuse.x = 0.f;
		pDesc->vDiffuse.y = 0.f;
		pDesc->vDiffuse.z = 0.f;
	}
	else
	{
		pDesc->vDiffuse.x -= m_fLightPower * fTimeDelta * 10.f;
		pDesc->vDiffuse.y -= m_fLightPower * fTimeDelta * 10.f;
		pDesc->vDiffuse.z -= m_fLightPower * fTimeDelta * 10.f;
	}


}

void CFlaskLight::LateTick(_float fTimeDelta)
{

}

HRESULT CFlaskLight::Render()
{


	Render_Col();

	return S_OK;
}



HRESULT CFlaskLight::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	///* For.Com_Renderer */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	//	return E_FAIL;

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, , TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;





	return S_OK;
}



void CFlaskLight::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

}



CFlaskLight * CFlaskLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFlaskLight*		pInstance = new CFlaskLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFlaskLight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlaskLight::Clone(void * pArg)
{
	CFlaskLight*		pInstance = new CFlaskLight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFlaskLight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CFlaskLight::Free()
{
	__super::Free();


	Safe_Release(m_pTransformCom);
}
