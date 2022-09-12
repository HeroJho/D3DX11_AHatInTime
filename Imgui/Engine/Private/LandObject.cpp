#include "..\Public\LandObject.h"

#include "Object_Manager.h"
#include "VIBuffer.h"
#include "Transform.h"

CLandObject::CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLandObject::CLandObject(const CLandObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLandObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_LandDesc, pArg, sizeof(LANDDESC));

	return S_OK;
}

void CLandObject::Tick(_float fTimeDelta)
{
}

void CLandObject::LateTick(_float fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return S_OK;
}

HRESULT CLandObject::SetUp_OnTerrain(CTransform* pTransform, _float fMagicNumber)
{
	//CObject_Manager*		pObjectMgr = CObject_Manager::Get_Instance();

	//Safe_AddRef(pObjectMgr);

	//CVIBuffer*			pVIBuffer = (CVIBuffer*)pObjectMgr->Get_ComponentPtr(m_LandDesc.iTerrainLevelIndex, m_LandDesc.pLayerTag, m_LandDesc.pTerrainBufferComTag, m_LandDesc.iTerrainObjectIndex);

	//_float3		vTargetPos = pTransform->Get_State(CTransform::STATE_POSITION);

	//vTargetPos.y = pVIBuffer->Compute_Height(vTargetPos) + fMagicNumber;

	//pTransform->Set_State(CTransform::STATE_POSITION, vTargetPos);

	//Safe_Release(pObjectMgr);

	return S_OK;
}

void CLandObject::Free()
{
	__super::Free();

}
