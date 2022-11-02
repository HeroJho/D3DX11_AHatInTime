#include "stdafx.h"
#include "..\Public\Wisp.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "GameManager.h"

CWisp::CWisp(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CWisp::CWisp(const CWisp & rhs)
	: CGameObject(rhs)
{
}






HRESULT CWisp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWisp::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Wisp";



	WISPDESC* pDesc = (WISPDESC*)pArg;

	m_fRatio = pDesc->fRatio;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Set_CurSpeed(0.f);

	return S_OK;
}










void CWisp::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (0.f > m_fRatio)
	{
		m_fRatio = 0.f;
		CGameManager::Get_Instance()->Set_Wisp(false, 0, _float3(0.f, 0.f, 0.f));
	}


	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	CGameManager::Get_Instance()->Set_Wisp(true ,m_fRatio, vPos);
}



void CWisp::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);




}

HRESULT CWisp::Render()
{



	return S_OK;
}

void CWisp::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{



}














HRESULT CWisp::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	///* For.Com_Renderer */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	//	return E_FAIL;

	///* For.Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Mad_Crow"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;



	///* For.Com_Collider */
	//CCollider::COLLIDERDESC ColDesc;
	//ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	//ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(3.f, 3.f, 10.f);
	//if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
	//	return E_FAIL;





	return S_OK;
}










CWisp * CWisp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWisp*		pInstance = new CWisp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWisp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWisp::Clone(void * pArg)
{
	CWisp*		pInstance = new CWisp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CWisp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWisp::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
