#include "stdafx.h"
#include "..\Public\VSnatcher.h"
#include "GameInstance.h"

#include "DataManager.h"
#include "ToolManager.h"
#include "CamManager.h"
#include "ItemManager.h"

#include "Player.h"
#include "Camera_Free.h"
#include "ExPlo.h"
#include "Magic.h"
#include "Swip.h"
#include "StatuePosed_Boss.h"
#include "PuzzleCube_Boss.h"

CVSnatcher::CVSnatcher(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CVSnatcher::CVSnatcher(const CVSnatcher & rhs)
	: CGameObject(rhs)
{
}






HRESULT CVSnatcher::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVSnatcher::Initialize(void * pArg)
{
	// __super::Initialize(pArg);

 	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Snatcher";

	list<ANIM_LINEAR_DATA> LinearDatas = CDataManager::Get_Instance()->Load_Anim("VSnatcher");
	for (auto& Data : LinearDatas)
		Set_AnimLinearData(Data);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-60.57f, 0.101f, -115.45f, 1.f));
	m_pTransformCom->Set_CurSpeed(1.f);

	m_pTransformCom->Set_RealOriScale(XMVectorSet(1.5f, 1.5f, 1.5f, 1.f));

	XMStoreFloat3(&m_vSentorPos, XMVectorSet(-60.57f, 0.101f, -115.45f, 1.f));

	// Set_State(STATE_DISAPPEAR); 
	Set_State(STATE_CUT_1);
	
	 CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	 m_pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr(LEVEL_STATIC, TEXT("Layer_Player"), 0);
	 Safe_AddRef(m_pPlayer);
	 RELEASE_INSTANCE(CGameInstance);


	 m_iNaviIndex = CToolManager::Get_Instance()->Find_NaviIndex(XMVectorSet(-60.57f, 0.101f, -115.45f, 1.f));

	return S_OK;
}







void CVSnatcher::Set_AnimLinearData(ANIM_LINEAR_DATA LinearData)
{
	m_pModelCom->Push_AnimLinearData(LinearData);
}

void CVSnatcher::Set_State(STATE eState)
{
	m_ePreState = m_eState;
	m_eState = eState;


	if (m_ePreState != m_eState)
	{
		switch (m_eState)
		{
		case STATE_APPEAR:
			break;
		case STATE_SOFTAPPEAR:
			break;
		case STATE_DISAPPEAR:
			break;
		case STATE_IDLE:
			break;
		case STATE_TALKING:
			break;
		case STATE_CURSESTART:
			m_fCurseTimeAcc = 0.f;
			m_iCurseCount = m_iCurseMaxCount;
			break;
		case STATE_CURSE:
			break;
		case STATE_MINON:
			m_fMinonTimeAcc = 0.f;
			break;
		case STATE_MAGICSTART:
			break;
		case STATE_MAGIC:
			m_fMagicTimeAcc = 0.f;
			Create_Magic(50);
			break;
		case STATE_HOITSTART:
			break;
		case STATE_HOIT:
			m_iHoItCount = m_iHoItMaxCount;
			m_fHoItTimeAcc = 0.f;
			m_bIsUp = true;
			CCamManager::Get_Instance()->Get_Cam()->Set_State(CCamera_Free::CAM_GAME);
			break;
		case STATE_SWIPSTART:
			Create_Statue();
			Create_CubeBox();
			break;
		case STATE_SWIPS:
			m_fSwipTimeAcc = 0.f;
			break;
		case STATE_SNAPHAT:
			Choose_SnapHat();
			m_fSnapHatTimeAcc = 0.f;
			break;


		case STATE_CUT_1:
			// 위치
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-67.41f, 10.f, 111.f, 1.f));
			break;
		case STATE_CUT_2:
			break;

		default:
			break;
		}
	}

	Set_Anim();
}

void CVSnatcher::Set_Anim()
{

	switch (m_eState)
	{
	case STATE_APPEAR:
		m_pModelCom->Set_AnimIndex(0);
		break;
	case STATE_SOFTAPPEAR:
		m_pModelCom->Set_AnimIndex(18);
		break;
	case STATE_DISAPPEAR:
		m_pModelCom->Set_AnimIndex(9);
		break;
	case STATE_IDLE:
		m_pModelCom->Set_AnimIndex(24);
		break;
	case STATE_TALKING:
		m_pModelCom->Set_AnimIndex(25);
		break;
	case STATE_CURSESTART:
		m_pModelCom->Set_AnimIndex(6);
		break;
	case STATE_CURSE:
		m_pModelCom->Set_AnimIndex(7);
		break;
	case STATE_MINON:
		m_pModelCom->Set_AnimIndex(11);
		break;
	case STATE_MAGICSTART:
		m_pModelCom->Set_AnimIndex(24);
		break;
	case STATE_MAGIC:
		m_pModelCom->Set_AnimIndex(24);
		break;
	case STATE_HOITSTART:
		m_pModelCom->Set_AnimIndex(24);
		break;
	case STATE_HOIT:
		m_pModelCom->Set_AnimIndex(24);
		break;
	case STATE_SWIPSTART:
		m_pModelCom->Set_AnimIndex(24);
		break;
	case STATE_SWIPS:
		m_pModelCom->Set_AnimIndex(24);
		break;
	case STATE_SNAPHAT:
		m_pModelCom->Set_AnimIndex(21, true);
		break;
	default:
		break;
	}


}


void CVSnatcher::Compute_Pattern(_float fTimeDelta)
{
	_uint iRendNum = CToolManager::Get_Instance()->Get_RendomNum_Int(0, 2);

	switch (iRendNum)
	{
	case 0:
		 // Set_State(STATE_CURSESTART);
		break;
	case 1:
		// Set_State(STATE_MAGICSTART);
		break;
	case 2:
		Set_State(STATE_SNAPHAT);
		break;
	default:
		break;
	}

}

void CVSnatcher::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);


	switch (m_eState)
	{
	case STATE_APPEAR:
		Tick_Appear(fTimeDelta);
		break;
	case STATE_SOFTAPPEAR:
		Tick_SoftAppear(fTimeDelta);
		break;
	case STATE_DISAPPEAR:
		Tick_DisAppear(fTimeDelta);
		break;
	case STATE_IDLE:
		Tick_Idle(fTimeDelta);
		break;
	case STATE_TALKING:
		Tick_Talking(fTimeDelta);
		break;
	case STATE_CURSESTART:
		Tick_CurseStart(fTimeDelta);
		break;
	case STATE_CURSE:
		Tick_Curse(fTimeDelta);
		break;
	case STATE_MINON:
		Tick_Minon(fTimeDelta);
		break;
	case STATE_MAGICSTART:
		Tick_MagicStart(fTimeDelta);
		break;
	case STATE_MAGIC:
		Tick_Magic(fTimeDelta);
		break;
	case STATE_HOITSTART:
		Tick_HoItStart(fTimeDelta);
		break;
	case STATE_HOIT:
		Tick_HoIt(fTimeDelta);
		break;
	case STATE_SNAPHAT:
		Tick_SnapHat(fTimeDelta);
		break;
	case STATE_SWIPSTART:
		Tick_SwipStart(fTimeDelta);
		break;
	case STATE_SWIPS:
		Tick_Swip(fTimeDelta);
		break;

	case STATE_CUT_1:
		Tick_Cut_1(fTimeDelta);
		break;
	case STATE_CUT_2:
		Tick_Cut_2(fTimeDelta);
		break;
	default:
		break;
	}


}

void CVSnatcher::Tick_Appear(_float fTimeDelta)
{
}

void CVSnatcher::Tick_SoftAppear(_float fTimeDelta)
{
}

void CVSnatcher::Tick_DisAppear(_float fTimeDelta)
{

	CTransform* pTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
	
	_vector vPlayerPos = pTran->Get_State(CTransform::STATE_POSITION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDir  = vPlayerPos - vMyPos;
	_vector vNorDir = XMVector3Normalize(vDir);
	_float fDis = XMVectorGetX(XMVector3Length(vDir));

	if (13.f > fDis)
	{
		CCamManager::Get_Instance()->Get_Cam()->Set_Target(this);
		CCamManager::Get_Instance()->Get_Cam()->Set_State(CCamera_Free::CAM_BOSS);
		Set_State(STATE_SOFTAPPEAR);
	}


}

void CVSnatcher::Tick_Idle(_float fTimeDelta)
{

	Compute_Pattern(fTimeDelta);

}

void CVSnatcher::Tick_Talking(_float fTimeDelta)
{
}

void CVSnatcher::Tick_CurseStart(_float fTimeDelta)
{
	_vector vPlayerPos = ((CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt_ForLandObject(vPlayerPos);
}

void CVSnatcher::Tick_Curse(_float fTimeDelta)
{
	m_fCurseTimeAcc += fTimeDelta;

	if (0 > m_iCurseCount)
	{
		Set_State(STATE_IDLE);
		m_fCurseTimeAcc = 0.f;
		m_iCurseCount = 0;
		return;
	}

	_vector vPlayerPos = ((CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

	if (m_fCurseDelayTime < m_fCurseTimeAcc)
	{
		Create_ExPlo(vPlayerPos);
		m_fCurseTimeAcc = 0.f;
		--m_iCurseCount;
	}

	m_pTransformCom->LookAt_ForLandObject(vPlayerPos);
}

void CVSnatcher::Tick_Minon(_float fTimeDelta)
{
	m_fMinonTimeAcc += fTimeDelta;

	if (5.f < m_fMinonTimeAcc)
	{
		Set_State(STATE_IDLE);
		m_fMinonTimeAcc = 0.f;
	}

}

void CVSnatcher::Tick_MagicStart(_float fTimeDelta)
{
	// 모자를 바꾼다
}

void CVSnatcher::Tick_Magic(_float fTimeDelta)
{
	m_fMagicTimeAcc += fTimeDelta;

	if (m_fMagicEndTime < m_fMagicTimeAcc)
	{
		Set_State(STATE_IDLE);
		m_fMagicTimeAcc = 0.f;
	}

}

void CVSnatcher::Tick_HoItStart(_float fTimeDelta)
{
	_vector vPlayerPos = ((CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt_ForLandObject(vPlayerPos);
}

void CVSnatcher::Tick_HoIt(_float fTimeDelta)
{
	_vector vPlayerPos = ((CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt_ForLandObject(vPlayerPos);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (m_bIsUp)
	{
		if (-10.f < XMVectorGetY(vPos))
		{
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 10.f, fTimeDelta);
			m_fHoItTimeAcc = 0.f;
		}
		else
		{
			vPos = XMVectorSetY(vPos, -10.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));
			
			m_fHoItTimeAcc += fTimeDelta;
			if (1.f < m_fHoItTimeAcc)
			{
				m_bIsUp = false;
				m_fHoItTimeAcc = 0.f;
				if (0 != m_iHoItCount)
				{
					_float fX = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
					_float fZ = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
					_float fDis = CToolManager::Get_Instance()->Get_RendomNum(4.f, 12.f);
					_vector vNorDir = XMVector3Normalize(XMVectorSet(fX, -10.f, fZ, 0.f));

					_vector vPos = XMLoadFloat3(&m_vSentorPos) + vNorDir * fDis;
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));
				}
				else
				{
					_vector vPos = XMLoadFloat3(&m_vSentorPos);
					vPos = XMVectorSetY(vPos, -10.f);
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));
				}

			}

		}
	}
	else
	{
		if (0.f > XMVectorGetY(vPos))
		{
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 10.f, fTimeDelta);
			m_fHoItTimeAcc = 0.f;
		}
		else
		{
			vPos = XMVectorSetY(vPos, 0.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));
			m_fHoItTimeAcc += fTimeDelta;
			if (1.f < m_fHoItTimeAcc)
			{
				m_bIsUp = true;
				m_fHoItTimeAcc = 0.f;
				--m_iHoItCount;

				if (0 > m_iHoItCount)
				{
					CCamManager::Get_Instance()->Get_Cam()->Set_State(CCamera_Free::CAM_BOSS);
					Set_State(STATE_IDLE);
					m_bIsUp = true;
					m_iHoItCount = 0;
					m_fHoItTimeAcc = 0.f;
				}

			}
		}
	}
}

void CVSnatcher::Tick_SwipStart(_float fTimeDelta)
{


}

void CVSnatcher::Tick_Swip(_float fTimeDelta)
{
	m_fSwipTimeAcc += fTimeDelta;

	if (20.f < m_fSwipTimeAcc && 29.f > m_fSwipTimeAcc)
	{
		if (CSwip::STATE_IDLE == m_pSwip->Get_State())
			m_pSwip->Start();
		m_fSwipTimeAcc = 30.f;
	}
	else if (40.f < m_fSwipTimeAcc && 49.f > m_fSwipTimeAcc)
	{
		Set_State(STATE_IDLE);
		m_fSwipTimeAcc = 50.f;
	}

}

void CVSnatcher::Tick_SnapHat(_float fTimeDelta)
{
	_vector vPlayerPos = ((CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt_ForLandObject(vPlayerPos);

	m_fSnapHatTimeAcc += fTimeDelta;

	if (3.f < m_fSnapHatTimeAcc && 9.f > m_fSnapHatTimeAcc)
	{
		char cTemp[MAX_PATH];
		TCHAR szTemp[MAX_PATH];
		strcpy_s(cTemp, m_sSnapTag.data());
		CToolManager::Get_Instance()->CtoTC(cTemp, szTemp);
		CItemManager::Get_Instance()->Delete_Hat(szTemp);

		Equip_Sockat(m_sSnapTag, SLOT_HAND);
		m_fSnapHatTimeAcc = 10.f;
	}
	else if (11.5f < m_fSnapHatTimeAcc && 19.f > m_fSnapHatTimeAcc)
	{
		Drop_Hat();
		m_pSockatCom->Remove_Sockat(SLOT_HAND);
		Equip_Sockat(m_sSnapTag, SLOT_HEAD);
		m_fSnapHatTimeAcc = 20.f;
	}
}

void CVSnatcher::Tick_Cut_1(_float fTimeDelta)
{
}

void CVSnatcher::Tick_Cut_2(_float fTimeDelta)
{
}







void CVSnatcher::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);

	if (nullptr == m_pRendererCom)
		return;


	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	Tick_Col(mWorld);



	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		End_Anim();
	}


	// 소켓 갱신
	m_pSockatCom->Tick(fTimeDelta, m_pTransformCom);
	m_pSockatCom->LateTick(fTimeDelta, m_pRendererCom);


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);

	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	if (true == isDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	RELEASE_INSTANCE(CGameInstance);
}

void CVSnatcher::End_Anim()
{
	switch (m_eState)
	{
	case STATE_APPEAR:
		break;
	case STATE_SOFTAPPEAR:
		Set_State(STATE_IDLE);
		break;
	case STATE_DISAPPEAR:
		break;
	case STATE_IDLE:
		break;
	case STATE_TALKING:
		break;
	case STATE_CURSESTART:
		Set_State(STATE_CURSE);
		break;
	case STATE_CURSE:
		break;
	case STATE_MAGICSTART:
		Set_State(STATE_MAGIC);
		break;
	case STATE_MAGIC:
		break;
	case STATE_HOITSTART:
		Set_State(STATE_HOIT);
		break;
	case STATE_HOIT:
		break;
	case STATE_SNAPHAT:
	{
		if ("Ori_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HEAD))
		{
			Set_State(STATE_CURSESTART);
		}
		else if ("Sprint_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HEAD))
		{
			Set_State(STATE_HOITSTART);
		}
		else if ("Witch_Hat" == m_pSockatCom->Get_SlotTag(SLOT_HEAD))
		{
			Set_State(STATE_MAGICSTART);
		}
		else if ("Mask_Cat" == m_pSockatCom->Get_SlotTag(SLOT_HEAD))
		{
			Set_State(STATE_SWIPSTART);
		}
		else if ("Mask_Fox" == m_pSockatCom->Get_SlotTag(SLOT_HEAD))
		{
			Set_State(STATE_IDLE);
		}
	}
		break;
	case STATE_SWIPSTART:
		Set_State(STATE_SWIPS);
		break;
	case STATE_SWIPS:
		break;
	}
}







void CVSnatcher::Create_ExPlo(_fvector vPos)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float3 vTempPos;
	XMStoreFloat3(&vTempPos, vPos);

	CExPlo::EXPLODESC Desc;
	Desc.vPos = vTempPos;

	pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_ExPlo"), LEVEL_BOSS, TEXT("Layer_Ex"), &Desc);

	RELEASE_INSTANCE(CGameInstance);
}

void CVSnatcher::Create_Magic(_uint iCount)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMagic::MAGICDESC Desc;
	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	vPos.y += 5.f;
	Desc.vSentorPos = vPos;

	_float fGoTime = 3.f;
	_float fAngle = 0.f; 
	for (_uint i = 0; i < iCount; ++i)
	{
		_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
		_vector vNorDir = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f), RotationMatrix));
		_vector vStartPos = XMLoadFloat3(&vPos) + vNorDir * 30.f;

		Desc.fGoTime = fGoTime;
		XMStoreFloat3(&Desc.vStartPos, vStartPos);
		pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Magic"), LEVEL_BOSS, TEXT("Layer_Ex"), &Desc);
		
		fGoTime += 0.05f;
		fAngle += 360.f / iCount;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CVSnatcher::Create_Statue()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < 3; ++i)
	{
		_float fX = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
		_float fZ = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
		_float fDis = CToolManager::Get_Instance()->Get_RendomNum(4.f, 12.f);
		_vector vNorDir = XMVector3Normalize(XMVectorSet(fX, -10.f, fZ, 0.f));

		_vector vPos = XMLoadFloat3(&m_vSentorPos) + vNorDir * fDis;


		CStatuePosed_Boss::STATUEDESC StatueDesc;
		XMStoreFloat3(&StatueDesc.vPos, vPos);
		StatueDesc.vPos.y += 3.f;
		StatueDesc.vRotation = _float3(0.f, fX, 0.f);
		StatueDesc.pTarget = pGameInstance->Get_GameObjectPtr(LEVEL_STATIC, TEXT("Layer_Player"), 0);

		pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StatuePosed_Boss"), LEVEL_BOSS, TEXT("Layer_Monster"), &StatueDesc);


	}

	RELEASE_INSTANCE(CGameInstance);
}

void CVSnatcher::Create_CubeBox()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < 6; ++i)
	{
		_float fX = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
		_float fZ = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
		_float fDis = CToolManager::Get_Instance()->Get_RendomNum(4.f, 12.f);
		_vector vNorDir = XMVector3Normalize(XMVectorSet(fX, -10.f, fZ, 0.f));

		_vector vPos = XMLoadFloat3(&m_vSentorPos) + vNorDir * fDis;


		CPuzzleCube_Boss:: PUZZLECUBEDESC StatueDesc;
		XMStoreFloat3(&StatueDesc.vPos, vPos);
		StatueDesc.vPos.y += 3.f;

		pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PuzzleCube_Boss"), LEVEL_BOSS, TEXT("Layer_Cube"), &StatueDesc);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CVSnatcher::Choose_SnapHat()
{
	if (CItemManager::Get_Instance()->Get_Hats()->empty())
	{
		Set_State(STATE_IDLE);
		return;
	}

	_int iSnapIndex = m_iSnapIndex;
	_bool bIsHave = false;
	while (true)
	{
		iSnapIndex = CToolManager::Get_Instance()->Get_RendomNum_Int(0, 4);
		if (m_iSnapIndex != iSnapIndex)
		{
			switch (iSnapIndex)
			{
			case 0:
				bIsHave = CItemManager::Get_Instance()->Check_Hat(TEXT("Ori_Hat"));
				if (bIsHave)
					m_sSnapTag = "Ori_Hat";
				break;
			case 1:
				bIsHave = CItemManager::Get_Instance()->Check_Hat(TEXT("Sprint_Hat"));
				if (bIsHave)
					m_sSnapTag = "Sprint_Hat";
				break;
			case 2:
				bIsHave = CItemManager::Get_Instance()->Check_Hat(TEXT("Witch_Hat"));
				if (bIsHave)
					m_sSnapTag = "Witch_Hat";
				break;
			case 3:
				bIsHave = CItemManager::Get_Instance()->Check_Hat(TEXT("Mask_Cat"));
				if (bIsHave)
					m_sSnapTag = "Mask_Cat";
				break;
			case 4:
				bIsHave = CItemManager::Get_Instance()->Check_Hat(TEXT("Mask_Fox"));
				if (bIsHave)
					m_sSnapTag = "Mask_Fox";
				break;
			}
		}

		if (bIsHave)
			break;
	}


	m_iSnapIndex = iSnapIndex;
	

}

void CVSnatcher::Drop_Hat()
{
	string sTemp = m_pSockatCom->Get_SlotTag(SLOT_HEAD);

	if ("" == sTemp)
		return;

	char cTempName[MAX_PATH];
	strcpy_s(cTempName, sTemp.data());
	TCHAR czTempName[MAX_PATH];
	CToolManager::Get_Instance()->CtoTC(cTempName, czTempName);

	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	vPos.y += 5.f;

	_float3 vDir;
	_float fPow;
	_float fX = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
	_float fY = 0.f;
	_float fZ = CToolManager::Get_Instance()->Get_RendomNum(-100.f, 100.f);
	XMStoreFloat3(&vDir, XMVector3Normalize(XMVectorSet(fX, fY, fZ, 0.f)));
	fPow = CToolManager::Get_Instance()->Get_RendomNum(1.5f, 2.f);

	CItemManager::Get_Instance()->Make_DrowItem(TEXT("Prototype_GameObject_Hat"), czTempName, LEVEL_BOSS, vPos, _float3(0.f, 0.f, 0.f), _float3(1.f, 1.f, 1.f), 1, vDir, fPow, 5.f, m_iNaviIndex);

}

_float3 CVSnatcher::Get_PacePos()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vNorLookDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vNorRightDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	
	_matrix mUp = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f));
	_matrix mRight = XMMatrixRotationAxis(-vNorRightDir, XMConvertToRadians(45.f));

	_matrix mTotal = mUp * mRight;

	vNorLookDir = XMVector3Normalize(XMVector3TransformNormal(vNorLookDir, mTotal));

	_float3 vVPos;
	vPos += vNorLookDir * 5.f;
	XMStoreFloat3(&vVPos, vPos);
	return vVPos;
}

_float3 CVSnatcher::Get_PaceLook()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 5.f);


	_float3 vVPos;
	XMStoreFloat3(&vVPos, vPos);
	return vVPos;
}







HRESULT CVSnatcher::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}


	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();


	return S_OK;
}

void CVSnatcher::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if (!strcmp("Near_Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
		{

			_float3 vCamPos, vLookPos;
			vCamPos = Get_PacePos();
			vLookPos = Get_PaceLook();

			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(vCamPos, vLookPos, false);

		}

	}
}














HRESULT CVSnatcher::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("VSnatcher"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(Ready_Sockat()))
		return E_FAIL;


	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));


	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	strcpy(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(5.f, 5.f, 5.f);
	strcpy(ColDesc.sTag, "Near_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;




	CSwip::WISPDESC WispDesc;
	WispDesc.pOwner = this;
	WispDesc.fMaxRatio = 15.f;
	WispDesc.fSpeed = 0.5f;

	CGameObject* pObj = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Swip"), LEVEL_STATIC, TEXT("Layer_Swip"), &pObj, &WispDesc)))
		return E_FAIL;
	m_pSwip = (CSwip*)pObj;
	Safe_AddRef(m_pSwip);
	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}



HRESULT CVSnatcher::Ready_Sockat()
{
	/* For.Com_Sockat */
	CSockat::SOCATDESC SockatDesc;
	ZeroMemory(&SockatDesc, sizeof(CSockat::SOCATDESC));
	XMStoreFloat4x4(&SockatDesc.mPivot, m_pModelCom->Get_PivotMatrix());
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sockat"), TEXT("Com_Sockat"), (CComponent**)&m_pSockatCom, &SockatDesc)))
		return E_FAIL;


	if (FAILED(m_pSockatCom->Match_Enum_BoneName("Head4", SLOT_HEAD)))
		return E_FAIL;
	if (FAILED(m_pSockatCom->Match_Enum_BoneName("R-Hand-Finger01", SLOT_HAND)))
		return E_FAIL;




	// Equip_Sockat(string("Witch_Hat"), SLOT_HEAD);



	return S_OK;
}



HRESULT CVSnatcher::Equip_Sockat(string sItemName, SLOT eSlot)
{

	// 중복으로 착용하려고 그럼 나간다
	if (m_pSockatCom->Check_IsHaveSocket(eSlot, sItemName))
		return S_OK;

	// Item을 착용 중이냐
	// 그럼 기존거를 삭제하고 끼운다
	if (!m_pSockatCom->Check_Sockat(eSlot))
		m_pSockatCom->Remove_Sockat(eSlot);


	TCHAR cItemName[MAX_PATH];
	CToolManager::Get_Instance()->CtoTC(sItemName.data(), cItemName);


	CSockat::PARTSDESC PartsDesc;
	ZeroMemory(&PartsDesc, sizeof(CSockat::PARTSDESC));


	if (!lstrcmp(cItemName, TEXT("Ori_Hat")))
	{
		if (SLOT_HAND == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Ori_Hat"));
			PartsDesc.vPos = _float3(-3.81, 0.24f, -5.68f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-90.f, 0.f, 0.f);
			PartsDesc.pOwner = this;
		}
		else if (SLOT_HEAD == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Ori_Hat"));
			PartsDesc.vPos = _float3(0.37f, 0.f, -6.74f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-86.6f, -41.9f, 0.f);
			PartsDesc.pOwner = this;
		}
	}
	else if (!lstrcmp(cItemName, TEXT("Sprint_Hat")))
	{
		if (SLOT_HAND == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Sprint_Hat"));
			PartsDesc.vPos = _float3(-3.81, 0.24f, -5.68f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-90.f, 0.f, 0.f);
			PartsDesc.pOwner = this;
		}
		else if (SLOT_HEAD == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Sprint_Hat"));
			PartsDesc.vPos = _float3(0.37f, 0.f, -6.74f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-86.6f, -41.9f, 0.f);
			PartsDesc.pOwner = this;
		}

	}
	else if (!lstrcmp(cItemName, TEXT("Witch_Hat")))
	{
		if (SLOT_HAND == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Witch_Hat"));
			PartsDesc.vPos = _float3(-3.81, 0.24f, -5.68f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-90.f, 0.f, 0.f);
			PartsDesc.pOwner = this;
		}
		else if (SLOT_HEAD == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Witch_Hat"));
			PartsDesc.vPos = _float3(0.37f, 0.f, -6.74f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-86.6f, -41.9f, 0.f);
			PartsDesc.pOwner = this;
		}
	}
	else if (!lstrcmp(cItemName, TEXT("Mask_Cat")))
	{
		if (SLOT_HAND == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Mask_Cat"));
			PartsDesc.vPos = _float3(-3.81, 0.24f, -5.68f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-90.f, 0.f, 0.f);
			PartsDesc.pOwner = this;
		}
		else if (SLOT_HEAD == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Mask_Cat"));
			PartsDesc.vPos = _float3(0.37f, 0.f, -6.74f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-86.6f, -41.9f, 0.f);
			PartsDesc.pOwner = this;
		}
	}
	else if (!lstrcmp(cItemName, TEXT("Mask_Fox")))
	{
		if (SLOT_HAND == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Mask_Fox"));
			PartsDesc.vPos = _float3(-3.81, 0.24f, -5.68f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-90.f, 0.f, 0.f);
			PartsDesc.pOwner = this;
		}
		else if (SLOT_HEAD == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Mask_Fox"));
			PartsDesc.vPos = _float3(0.37f, 0.f, -6.74f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-86.6f, -41.9f, 0.f);
			PartsDesc.pOwner = this;
		}
	}




	if (!FAILED(m_pSockatCom->Add_Sockat(eSlot, m_pModelCom, TEXT("Prototype_GameObject_Parts"), PartsDesc)))
		return E_FAIL;

	return S_OK;
}





CVSnatcher * CVSnatcher::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVSnatcher*		pInstance = new CVSnatcher(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVSnatcher"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CVSnatcher::Clone(void * pArg)
{
	CVSnatcher*		pInstance = new CVSnatcher(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVSnatcher"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVSnatcher::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pSwip);

	Safe_Release(m_pSockatCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
