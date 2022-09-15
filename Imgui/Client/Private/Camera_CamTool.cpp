#include "stdafx.h"
#include "..\Public\Camera_CamTool.h"

#include "GameInstance.h"
#include "CamManager.h"

CCamera_CamTool::CCamera_CamTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_CamTool::CCamera_CamTool(const CCamera_CamTool & rhs, CTransform::TRANSFORMDESC * pArg)
	: CCamera(rhs, pArg)
{

}








_vector CCamera_CamTool::Get_Pos()
{
	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

void CCamera_CamTool::Set_Pos(_float3 vPos)
{
	_vector vTempPos = XMLoadFloat3(&vPos);
	vTempPos = XMVectorSetW(vTempPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTempPos);
}

_bool CCamera_CamTool::Move(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimitDistance)
{
	return m_pTransformCom->Move(vTargetPos, fSpeed, fTimeDelta, fLimitDistance);
}








HRESULT CCamera_CamTool::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_CamTool::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	return S_OK;
}

void CCamera_CamTool::Tick(_float fTimeDelta)
{
	if (CCamManager::Get_Instance()->Get_Start())
	{

	}
	else
	{
		Input(fTimeDelta);
	}


	__super::Tick(fTimeDelta);

}

void CCamera_CamTool::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera_CamTool::Render()
{
	return S_OK;
}








void CCamera_CamTool::Input(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKState(DIK_W) & 0x80)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (pGameInstance->Get_DIKState(DIK_S) & 0x80)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (pGameInstance->Get_DIKState(DIK_A) & 0x80)
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (pGameInstance->Get_DIKState(DIK_D) & 0x80)
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	_long	MouseMove = 0;


	if (pGameInstance->Mouse_Pressing(DIMK_RBUTTON))
	{
		if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))
		{
			m_pTransformCom->Turn(XMVECTOR{ 0.f, 1.f, 0.f }, MouseMove * fTimeDelta * 0.05f);
		}

		if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.05f);
		}
	}

	Safe_Release(pGameInstance);
}








CCamera_CamTool * CCamera_CamTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_CamTool*		pInstance = new CCamera_CamTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_CamTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_CamTool::Clone(void* pArg)
{
	CCamera::CAMERADESC*		pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CCamera_CamTool*		pInstance = new CCamera_CamTool(*this, &pCameraDesc->TransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CCamera_CamTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_CamTool::Free()
{
	__super::Free();


}
