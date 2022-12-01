#include "stdafx.h"
#include "..\Public\StaticModel_Instance.h"
#include "GameInstance.h"

#include "GameManager.h"
#include "ToolManager.h"


CStaticModel_Instance::CStaticModel_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CStaticModel_Instance::CStaticModel_Instance(const CStaticModel_Instance & rhs)
	: CGameObject(rhs)
{

}


HRESULT CStaticModel_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStaticModel_Instance::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	STATICMODELDESC* Desc = (STATICMODELDESC*)pArg;
	wcscpy_s(m_cModelTag, Desc->cModelTag);
	m_bWall = Desc->bWall;
	m_iTagID = Desc->iTagID;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CStaticModel_Instance::Tick(_float fTimeDelta)
{

}

void CStaticModel_Instance::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	// sub_thorn_wall_bloom, sub_thorn_wall_dead, forest_plant, harbour_fern
	// yardang_random_1~5

	_bool bRender = true;
	if (1 == CGameManager::Get_Instance()->Get_CurIndex() && 
		(!lstrcmp(TEXT("sub_thorn_wall_bloom_Instance"), m_cModelTag) ||
		!lstrcmp(TEXT("sub_thorn_wall_dead_Instance"), m_cModelTag) || 
		!lstrcmp(TEXT("forest_plant_Instance"), m_cModelTag) ||
		!lstrcmp(TEXT("harbour_fern_Instance"), m_cModelTag) ||
		!lstrcmp(TEXT("yardang_random_1_Instance"), m_cModelTag) ||
		!lstrcmp(TEXT("yardang_random_2_Instance"), m_cModelTag) || 
		!lstrcmp(TEXT("yardang_random_3_Instance"), m_cModelTag) || 
		!lstrcmp(TEXT("yardang_random_4_Instance"), m_cModelTag) || 
		!lstrcmp(TEXT("yardang_random_5_Instance"), m_cModelTag)  ))
	{
		bRender = false;
	}
	else if (2 == CGameManager::Get_Instance()->Get_CurIndex())
	{

	}


	if (!lstrcmp(TEXT("Fiona_Instance"), m_cModelTag))
		bRender = false;


	if (bRender)
	{

		if (lstrcmp(TEXT("SubCon_Instance"), m_cModelTag) && lstrcmp(TEXT("SubConBoss_Instance"), m_cModelTag)
		&& lstrcmp(TEXT("forest_plant_Instance"), m_cModelTag) && lstrcmp(TEXT("harbour_fern_Instance"), m_cModelTag))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);

		if (m_bWall && CGameManager::Get_Instance()->Get_WispInfoNum())
		{
			if (2 == m_iTagID)
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
			else
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		}
		else if (!m_bWall)
		{
			if (2 == m_iTagID)
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
			else
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		}
	}

}

HRESULT CStaticModel_Instance::Render()
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


	
	_uint iPassIndex = 0;
	_int iWispNum = CGameManager::Get_Instance()->Get_WispInfoNum();

	if (iWispNum && 1 != m_iTagID)
	{
		iPassIndex = 1;

		
		_bool* pIsDeleteSubCons = CGameManager::Get_Instance()->Get_DeleteSubCons();
		_float* pWispRatios = CGameManager::Get_Instance()->Get_WispRatios();
		_float4* pWispPoss = CGameManager::Get_Instance()->Get_WispPoss();

		


		char cTamp[MAX_PATH];
		CToolManager::Get_Instance()->TCtoC(m_cModelTag, cTamp);

		_bool bIsSubCon = false;
		if (!strcmp("SubCon_Instance", cTamp) || !strcmp("SubConBoss_Instance", cTamp))
		{
			bIsSubCon = true;

			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_GlassTexture", 0)))
				return E_FAIL;
		}

		
		if (FAILED(m_pShaderCom->Set_RawValue("g_WispInfoNum", &iWispNum, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_IsSubCon", &bIsSubCon, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_Wall", &m_bWall, sizeof(_bool))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_gIsDeleteSubCons", pIsDeleteSubCons, 256, 1)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_WispRatios", pWispRatios, 256, 2)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_WispPoss", pWispPoss, 256, 3)))
			return E_FAIL;

	}
	else
	{

		iPassIndex = 0;
		if (!lstrcmp(TEXT("Toilet_Area_Instance"), m_cModelTag))
			iPassIndex = 3;
	}




	_bool bSpec = false;

	if (!lstrcmp(m_cModelTag, TEXT("Branch_2_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("Branch_3_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("Branch_4_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("Branch_5_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("BranchG_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("BranchH_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("BranchI_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("Tree1_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("tree2_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("Tree3_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("tree_thin_01_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("tree_thin_02_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("tree_thin_03_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("tree_thin_04_Instance")) ||
		!lstrcmp(m_cModelTag, TEXT("tree_will_Instance")))
	{
		bSpec = true;
	}


	if (FAILED(m_pShaderCom->Set_RawValue("g_Spac", &bSpec, sizeof(_bool))))
		return E_FAIL;
	

	

	_bool bBlur = false;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bBlur", &bBlur, sizeof(_bool))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		// (!lstrcmp(m_cModelTag, TEXT("tree_will_Instance")) && 2 == i
		if ((!lstrcmp(m_cModelTag, TEXT("Lamppost_Instance")) && 1 == i) || (!lstrcmp(m_cModelTag, TEXT("ClockTower_Instance")) && 1 == i ))
		{
			if (1 != iPassIndex)
				iPassIndex = 4;
			else
				iPassIndex = 5;

			bBlur = true;
			if (FAILED(m_pShaderCom->Set_RawValue("g_bBlur", &bBlur, sizeof(_bool))))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CStaticModel_Instance::Render_ShadowDepth()
{

	if (nullptr == m_pShaderCom ||
		nullptr == m_pTransformCom)
		return E_FAIL;



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;


	//_vector vPos = XMVectorSetW(XMLoadFloat3(&pGameInstance->Get_PlayerPos()), 1.f);
	//_vector		vLightAt = XMVectorSetW(vPos, 1.f);
	//vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 5.f);
	//vPos = XMVectorSetX(vPos, XMVectorGetX(vPos) - 7.f);
	//_vector		vLightEye = XMVectorSetW(vPos, 1.f);
	//_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_ShadowLightViewMatrix(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;
	}


	return S_OK;
}







HRESULT CStaticModel_Instance::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model_Instance"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (LEVEL_BOSS == CToolManager::Get_Instance()->Get_CulLevel())
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_BOSS, m_cModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, m_cModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}





	return S_OK;
}

CStaticModel_Instance * CStaticModel_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStaticModel_Instance*		pInstance = new CStaticModel_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStaticModel_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStaticModel_Instance::Clone(void * pArg)
{
	CStaticModel_Instance*		pInstance = new CStaticModel_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStaticModel_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticModel_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
