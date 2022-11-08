#include "stdafx.h"
#include "..\Public\StaticModel.h"
#include "GameInstance.h"

#include "MapManager.h"
#include "MeshManager.h"
#include "ToolManager.h"

CStaticModel::CStaticModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(m_cModelTag, sizeof(TCHAR) * MAX_PATH);
	ZeroMemory(&m_vAxis, sizeof(_float3));

}

CStaticModel::CStaticModel(const CStaticModel & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(m_cModelTag, sizeof(TCHAR) * MAX_PATH);
	ZeroMemory(&m_vAxis, sizeof(_float3));

}


_uint CStaticModel::Get_NumMesh()
{
	return m_pModelCom->Get_NumMeshes();
}

_uint CStaticModel::Get_Mesh_NumPrimitives(_uint iIndex)
{
	return m_pModelCom->Get_Mesh_NumPrimitives(iIndex);
}

const VTXMODEL * CStaticModel::Get_Mesh_NonAnimVertices(_uint iIndex)
{
	return m_pModelCom->Get_Mesh_NonAnimVertices(iIndex);
}
const FACEINDICES32 * CStaticModel::Get_Mesh_Indices(_uint iIndex)
{
	return m_pModelCom->Get_Mesh_Indices(iIndex);
}

HRESULT CStaticModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStaticModel::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	STATICMODELDESC* Desc = (STATICMODELDESC*)pArg;
	wcscpy(m_cModelTag, Desc->cModelTag);


	if (FAILED(Ready_Components(Desc)))
		return E_FAIL;


	_vector vPos = XMLoadFloat3(&Desc->vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	_vector vScale = XMLoadFloat3(&Desc->vScale);
	vScale = XMVectorSetW(vScale, 1.f);

	if (!lstrcmp(TEXT("Tree1"), m_cModelTag) || !lstrcmp(TEXT("tree2"), m_cModelTag) || !lstrcmp(TEXT("Tree3"), m_cModelTag))
	{
		if (1.1f > XMVectorGetX(vScale) && 1.1f > XMVectorGetY(vScale) && 1.1f > XMVectorGetZ(vScale))
		{
			vScale = XMVectorSetY(vScale, XMVectorGetY(vScale) * 4.f);
			vScale = XMVectorSetX(vScale, XMVectorGetX(vScale) * 1.f);
			vScale = XMVectorSetZ(vScale, XMVectorGetZ(vScale) * 1.f);
		}

	}


	m_pTransformCom->Set_Scale(vScale);

	m_vAxis = Desc->vAngle;
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_vAxis.x, XMVectorSet(0.f, 1.f, 0.f, 0.f), m_vAxis.y, XMVectorSet(0.f, 0.f, 1.f, 0.f), m_vAxis.z);

	return S_OK;
}

void CStaticModel::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float3 vPoss[3];
	_float fMinDis = 0.f;
	

	if (!CMeshManager::Get_Instance()->Get_ClickVertexMode())
	{

		if (pGameInstance->Key_Pressing(DIK_SPACE))
		{
			m_fSpaceTimeAcc += fTimeDelta;
			if (1.f < m_fSpaceTimeAcc)
			{
				if (m_pModelCom->Picking(m_pTransformCom, &fMinDis, vPoss))
				{
					CMeshManager::Get_Instance()->Add_Cell(fMinDis, vPoss);
				}
			}
		}
		else
		{
			m_fSpaceTimeAcc = 0.f;
		}

		if (pGameInstance->Key_Down(DIK_SPACE))
		{
			if (m_pModelCom->Picking(m_pTransformCom, &fMinDis, vPoss))
			{
				CMeshManager::Get_Instance()->Add_Cell(fMinDis, vPoss);
			}
		}

	}



	if (pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		if (pGameInstance->Mouse_Pressing(DIMK_WHEEL))
		{
			if (m_sModelNum == CMapManager::Get_Instance()->Get_PickedCreatedString())
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}

		if (CMapManager::Get_Instance()->Check_CulList(m_cModelTag))
		{

			if (!lstrcmp(TEXT("SubCon"), m_cModelTag) && m_pModelCom->Picking(m_pTransformCom, &fMinDis, vPoss))
			{
				
				_vector vMouseDir = XMVector3Normalize(XMLoadFloat3(&CGameInstance::Get_Instance()->Get_MouseDir()));
				_vector vMousePos = XMLoadFloat3(&CGameInstance::Get_Instance()->Get_MousePos());
				CMapManager::Get_Instance()->Set_SubConPos(vMousePos + vMouseDir * fMinDis);
				
				if (!CMeshManager::Get_Instance()->Get_ClickVertexMode()
					&& CMapManager::Get_Instance()->Get_MouseCul())
				{
					CMapManager::Get_Instance()->Add_TempClickedModel(fMinDis, this);
				}
				else
					CMeshManager::Get_Instance()->Move_FreeVectexCube(fMinDis);

			}


			if (CMapManager::Get_Instance()->Get_MouseCul())
			{
				if (CMapManager::Get_Instance()->CullingMouse(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) 
					&& m_pModelCom->Picking(m_pTransformCom, &fMinDis, vPoss))
				{

					if (!CMeshManager::Get_Instance()->Get_ClickVertexMode())
						CMapManager::Get_Instance()->Add_TempClickedModel(fMinDis, this);
					else
						CMeshManager::Get_Instance()->Move_FreeVectexCube(fMinDis);
				}
			}
			else
			{
				if (m_pModelCom->Picking(m_pTransformCom, &fMinDis, vPoss))
				{

					if (!CMeshManager::Get_Instance()->Get_ClickVertexMode())
						CMapManager::Get_Instance()->Add_TempClickedModel(fMinDis, this);
					else
						CMeshManager::Get_Instance()->Move_FreeVectexCube(fMinDis);
				}
			}

	
		}
			
	}

	RELEASE_INSTANCE(CGameInstance);
	return;







	RELEASE_INSTANCE(CGameInstance);
}

void CStaticModel::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	// m_pModelCom->Play_Animation(fTimeDelta);
	
	if (CMapManager::Get_Instance()->Get_ColMode())
		Tick_Col(m_pTransformCom->Get_WorldMatrix());



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	RELEASE_INSTANCE(CGameInstance);

	if (true == isDraw || !lstrcmp(m_cModelTag, TEXT("SubCon")) || !lstrcmp(m_cModelTag, TEXT("SubCon_Navi")))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CStaticModel::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (!lstrcmp(m_cModelTag, TEXT("Fiona")))
	{
		if (CMapManager::Get_Instance()->Get_ColMode())
			Render_Col();
		return S_OK;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;



	string sTemp2 = CMapManager::Get_Instance()->Get_PickedCreatedString();
	_bool bRendPickingColor = CMapManager::Get_Instance()->Get_RendPickingColor();
	_bool bIsPicked = m_sModelNum == sTemp2 && bRendPickingColor;
	if (FAILED(m_pShaderCom->Set_RawValue("g_IsPicked", &bIsPicked, sizeof(_bool))))
		return E_FAIL;



	RELEASE_INSTANCE(CGameInstance);


	if (CMapManager::Get_Instance()->Get_RendCulMode())
	{
		ID3D11RasterizerState* m_WireFrame;
		D3D11_RASTERIZER_DESC temp;
		ZeroMemory(&temp, sizeof(D3D11_RASTERIZER_DESC));
		temp.FillMode = D3D11_FILL_WIREFRAME;
		temp.CullMode = D3D11_CULL_NONE;
		temp.DepthClipEnable = true;
		m_pDevice->CreateRasterizerState(&temp, &m_WireFrame);

		m_pContext->RSSetState(m_WireFrame);
		Safe_Release(m_WireFrame);
	}




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


	m_pContext->RSSetState(nullptr);


	if (CMapManager::Get_Instance()->Get_ColMode())
		Render_Col();

	return S_OK;
}

_bool CStaticModel::Check_Model()
{
	string sModelName = CMapManager::Get_Instance()->Get_PickedString();
	char cTemp[MAX_PATH];
	ZeroMemory(cTemp, sizeof(char) * MAX_PATH);
	CToolManager::Get_Instance()->TCtoC(m_cModelTag, cTemp);


	if (strcmp(cTemp, sModelName.data()))
		return false;

	return true;
}

CCollider::COLLIDERDESC CStaticModel::Get_ColInfo()
{
	CCollider::COLLIDERDESC Desc;
	ZeroMemory(&Desc, sizeof(CCollider::COLLIDERDESC));

	if (m_Colliders.empty())
		return Desc;

	return m_Colliders.front()->GetColDesc();
}

void CStaticModel::Set_ColInfo(CCollider::COLLIDERDESC Desc)
{
	if (m_Colliders.empty())
		return;

	m_Colliders.front()->Edit_Col(Desc);
}


HRESULT CStaticModel::Ready_Components(STATICMODELDESC* Desc)
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_cModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ColDesc.vCenter = Desc->vCenter;
	ColDesc.vRotation = Desc->vRotation;
	ColDesc.vSize = Desc->vSize;
	ColDesc.bWall = Desc->bWall;
	ColDesc.iTagID = Desc->iTagID;
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	return S_OK;
}

CStaticModel * CStaticModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStaticModel*		pInstance = new CStaticModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStaticModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStaticModel::Clone(void * pArg)
{
	CStaticModel*		pInstance = new CStaticModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStaticModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticModel::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
