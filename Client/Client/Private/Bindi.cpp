#include "stdafx.h"
#include "..\Public\Bindi.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "GameManager.h"

#include "Light.h"

CBindi::CBindi(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBindi::CBindi(const CBindi & rhs)
	: CGameObject(rhs)
{
}






HRESULT CBindi::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBindi::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	BINDIDESC* pDesc = (BINDIDESC*)pArg;


	m_sTag = "Tag_Bindi";


	m_vOriPos = pDesc->vPos;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));

	Set_State(STATE_HOVER);

	m_pTransformCom->Set_Scale(XMVectorSet(0.5f, 0.5f, 0.5f, 1.f));
	m_fOriScale = 0.5f;
	m_fRanMinScale = CToolManager::Get_Instance()->Get_RendomNum(0.02f, 0.5f);
	m_fRandTime = CToolManager::Get_Instance()->Get_RendomNum(1.f, 5.f);




	// 빛 정의
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vDiffuse = m_vDiffu = _float4(1.f, 212.f / 255.f, 0.f, 1.f);
	LightDesc.vAmbient = m_vAmbi = _float4(0.3f, 0.3f, 0.3f, 1.f);

	_float4 vPos;
	vPos.x = pDesc->vPos.x;
	vPos.y = pDesc->vPos.y;
	vPos.z = pDesc->vPos.z;
	vPos.w = 1.f;
	LightDesc.vPosition = vPos;
	LightDesc.fRange = 5.f;// CToolManager::Get_Instance()->Get_RendomNum(10.f, 10.f);

	pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, &m_pLight);

	RELEASE_INSTANCE(CGameInstance);




	m_fBasiT = 0.1f;
	m_fMoveMaxRange = 20.f;

	Comput_BasiPos();
	m_vTargetPos = CToolManager::Get_Instance()->GetBesierPos(m_vMovePos[0], m_vMovePos[1], m_vMovePos[2], m_vMovePos[3], m_fBasiT);



	return S_OK;
}


void CBindi::Set_State(STATE eState)
{
	if (eState != m_eState)
	{
		switch (eState)
		{
		case Client::CBindi::STATE_DOWN:
			break;
		case Client::CBindi::STATE_HOVER:
			break;
		case Client::CBindi::STATE_UP:
			break;
		default:
			break;
		}
	}

	m_eState = eState;
}

void CBindi::Comput_Rand(_float fTimeDelta)
{
	m_fRandTimeAcc += fTimeDelta;

	if (m_fRandTime < m_fRandTimeAcc)
	{
		m_fRandTime = CToolManager::Get_Instance()->Get_RendomNum(1.f, 5.f);
		m_fRanSpeed = CToolManager::Get_Instance()->Get_RendomNum(0.1f, 1.f);
		m_fRanMinScale = CToolManager::Get_Instance()->Get_RendomNum(0.01f, 0.5f);
		m_fMoveSpeed = CToolManager::Get_Instance()->Get_RendomNum(0.1f, 2.f);

		m_fRandTimeAcc = 0.f;
	}

}

void CBindi::Comput_BasiPos()
{
	m_fBasiT = 0.f;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	for (_uint i = 0; i < 4; ++i)
	{
		_float fRange = CToolManager::Get_Instance()->Get_RendomNum(1.f, m_fMoveMaxRange);

		_float fRanDirX = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
		_float fRanDirY = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
		_float fRanDirZ = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);

		_vector vDir = XMVectorSet(fRanDirX, fRanDirY, fRanDirZ, 0.f);
		vDir = XMVector3Normalize(vDir);
		
		XMStoreFloat3(&m_vMovePos[i], vPos + vDir * fRange);
	}
}




void CBindi::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	// 위치 동기화
	XMStoreFloat4(&m_pLight->Get_LightEditDesc()->vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	Comput_Rand(fTimeDelta);

	switch (m_eState)
	{
	case Client::CBindi::STATE_DOWN:
		Tick_Down(fTimeDelta);
		break;
	case Client::CBindi::STATE_HOVER:
		Tick_Hover(fTimeDelta);
		break;
	case Client::CBindi::STATE_UP:
		Tick_UP(fTimeDelta);
		break;
	default:
		break;
	}


}


void CBindi::Tick_Down(_float fTimeDelta)
{
}

void CBindi::Tick_UP(_float fTimeDelta)
{
}

void CBindi::Tick_Hover(_float fTimeDelta)
{
	// 크기
	_float3 vScale = m_pTransformCom->Get_Scale();

	if (m_bScaleTurn)
	{
		vScale.x += m_fRanSpeed * fTimeDelta;
		vScale.y += m_fRanSpeed * fTimeDelta;
		vScale.z += m_fRanSpeed * fTimeDelta;
	}
	else
	{
		vScale.x -= m_fRanSpeed * fTimeDelta;
		vScale.y -= m_fRanSpeed * fTimeDelta;
		vScale.z -= m_fRanSpeed * fTimeDelta;
	}



	if (m_fRanMinScale > vScale.x)
	{
		vScale.x = m_fRanMinScale;
		vScale.y = m_fRanMinScale;
		vScale.z = m_fRanMinScale;

		m_bScaleTurn = true;
	}
	else if (m_fOriScale < vScale.x)
	{
		vScale.x = m_fOriScale;
		vScale.y = m_fOriScale;
		vScale.z = m_fOriScale;

		m_bScaleTurn = false;
	}

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));


	// 크기에 따른 빛 쌔기
	_float fRatio = vScale.x / m_fOriScale;
	
	_float4 vDiffuColor(m_vDiffu.x * fRatio, m_vDiffu.y *fRatio, m_vDiffu.z * fRatio, 1.f);
	_float4 vAmbColor(m_vDiffu.x *fRatio, m_vDiffu.y *fRatio, m_vDiffu.z *fRatio, 1.f);
	m_pLight->Get_LightEditDesc()->vDiffuse = vDiffuColor;
	m_pLight->Get_LightEditDesc()->vAmbient = vAmbColor;



	// 이동
	if (1.f < m_fBasiT)
	{
		m_fBasiT = 0.f;
		Comput_BasiPos();
	}

	_vector vVTargetPos = XMLoadFloat3(&m_vTargetPos);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_vector vTargetDir = vVTargetPos - vMyPos;
	_float fTargetDis = XMVectorGetX(XMVector3Length(vTargetDir));

	vTargetDir = XMVector3Normalize(vTargetDir);
	m_pTransformCom->Go_Dir(XMVectorSetW(vTargetDir, 0.f), m_fMoveSpeed, fTimeDelta);

	if (0.1f > fTargetDis)
	{
		m_fBasiT += 0.1f;
		m_vTargetPos = CToolManager::Get_Instance()->GetBesierPos(m_vMovePos[0], m_vMovePos[1], m_vMovePos[2], m_vMovePos[3], m_fBasiT);
	}

	
	_vector vOriPos = XMLoadFloat3(&m_vOriPos);
	_float fOriDi = XMVectorGetX(XMVector3Length(vMyPos - vOriPos));
	if (m_fMoveMaxRange < fOriDi)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vOriPos, 1.f));

}




void CBindi::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 빌보드
	_matrix mCam = pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);
	_vector vCamPos = mCam.r[3];

	m_pTransformCom->LookAtInv(vCamPos, 0.f);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance);

}



HRESULT CBindi::Render()
{
	if (nullptr == m_pVIBufferCom ||
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


	_bool bBlur = true;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bBlur", &bBlur, sizeof(_bool))))
		return E_FAIL;


	_uint iPassIndex = 0;
	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(iPassIndex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

void CBindi::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{



}




















HRESULT CBindi::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_FX_Flare_01"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}










CBindi * CBindi::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBindi*		pInstance = new CBindi(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBindi"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBindi::Clone(void * pArg)
{
	CBindi*		pInstance = new CBindi(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBindi"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBindi::Free()
{
	__super::Free();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
