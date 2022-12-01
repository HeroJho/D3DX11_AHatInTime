#include "stdafx.h"
#include "..\Public\VSnatcher.h"
#include "GameInstance.h"

#include "DataManager.h"
#include "ToolManager.h"
#include "CamManager.h"
#include "ItemManager.h"
#include "UIManager.h"
#include "CutSceneManager.h"
#include "ParticleManager.h"

#include "Player.h"
#include "Camera_Free.h"
#include "EyeAttackGround.h"
#include "Magic.h"
#include "Swip.h"
#include "StatuePosed_Boss.h"
#include "PuzzleCube_Boss.h"
#include "Parts.h"
#include "CaulDron.h"
#include "Toilet_Scream.h"
#include "Splash_wave.h"
#include "FinLaser.h"


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



	
	 CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	 m_pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr(LEVEL_STATIC, TEXT("Layer_Player"), 0);
	 Safe_AddRef(m_pPlayer);
	 RELEASE_INSTANCE(CGameInstance);


	 m_iNaviIndex = CToolManager::Get_Instance()->Find_NaviIndex(XMVectorSet(-60.57f, 0.101f, -115.45f, 1.f));

	 Equip_Sockat("crown", SLOT_HEAD);

	 if (LEVEL_BOSS == CToolManager::Get_Instance()->Get_CulLevel())
	 {
		 // Set_State(STATE_DISAPPEAR);
		 Set_State(STATE_CUT_6);
		 Start_Dark();

		 CUIManager::Get_Instance()->Set_Target(this);
	 }
	 else if (LEVEL_ENDING == CToolManager::Get_Instance()->Get_CulLevel())
	 {
		 Set_State(STATE_CUT_7);
		 CUIManager::Get_Instance()->Set_Target(this);
	 }
	 else
	 {
		 Set_State(STATE_DISAPPEAR);
		 Set_State(STATE_CUT_0);
	 }
	 
	 m_CreatureDesc.iMaxHP = 10;
	 m_CreatureDesc.iHP = 1;

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
			Create_Magic(20);
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
		case STATE_ATTACK:
			break;
		case STATE_SNAPHAT:
			Choose_SnapHat();
			m_fSnapHatTimeAcc = 0.f;
			CUIManager::Get_Instance()->On_Text(TEXT("어떤 모자가 좋을까아~~"), 0.6f, 1.f, true, true);
			break;
		case STATE_SIT:
			Equip_Sockat("Snatcher_Chair", SLOT_SPIN);
			Equip_Sockat("Snatcher_Book", SLOT_HAND);
			break;
		case STATE_CANATTACKED:
			m_bAttacked = false;
			m_fAttackedTimeAcc = 0.f;
			m_fCanAttackedTimeAcc = 0.f;
			CUIManager::Get_Instance()->On_Text(TEXT("*허억!* 잠깐...!"), 0.6f, 1.f, true, true);
			break;
		case STATE_DEAD:
			m_fDeadTimeAcc = 0.f;
			m_iFinLaserCount = 0;
			break;



		case STATE_CUT_0:
			m_pModelCom->Set_AnimIndex(0);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-67.41f, 10.f, 111.f, 1.f));
			break;
		case STATE_CUT_1:
			// 위치
		{
			_float3 vCamPos, vLookPos;
			vCamPos = Get_PacePos(10.f, 10.f, 30.f);
			vLookPos = Get_PaceLook(4.5f);

			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(vCamPos, vLookPos, false);
		}
			break;
		case STATE_CUT_2:

			m_fCutTimeAcc_1 = 0.f;
			m_iTalkCount = 0;
			m_iCutIndex = 0;

			m_pModelCom->Set_AnimIndex(14);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-36.7f, 2.98f, 73.32f, 1.f));

			break;
		case STATE_CUT_3:

			m_fCutTimeAcc_1 = 0.f;
			m_iTalkCount = 0;
			m_iCutIndex = 0;

			m_pModelCom->Set_AnimIndex(18);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-24.41f, -0.3f, 46.115f, 1.f));

			break;
		case STATE_CUT_4:

			m_fCutTimeAcc_1 = 0.f;
			m_iTalkCount = 0;
			m_iCutIndex = 0;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-64.93f, 0.076f, 21.48f, 1.f));

			break;
		case STATE_CUT_5:

			m_fCutTimeAcc_1 = 0.f;
			m_iTalkCount = 0;
			m_iCutIndex = 0;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-60.36f, -10.07f, -71.66f, 1.f));
			m_pTransformCom->Set_Scale(XMVectorSet(5.f, 5.f, 5.f, 1.f));

			break;
		case STATE_CUT_6:

			m_fCutTimeAcc_1 = 0.f;
			m_iTalkCount = 0;
			m_iCutIndex = 0;

			Equip_Sockat("Snatcher_Chair", SLOT_SPIN);
			Equip_Sockat("Snatcher_Book", SLOT_HAND);

			m_pModelCom->Set_AnimIndex(15);

			break;
		case STATE_CUT_7:

			m_fCutTimeAcc_1 = 0.f;
			m_iTalkCount = 0;
			m_iCutIndex = 0;


			// Equip_Sockat("Snatcher_Chair", SLOT_SPIN);
			Equip_Sockat("Snatcher_Book", SLOT_HAND);

			m_pModelCom->Set_AnimIndex(15);
			Start_Dark();
			m_bDark = false;
			CUIManager::Get_Instance()->Set_WhiteBoard(false);
			CCamManager::Get_Instance()->Get_Cam()->End_Shake();
			CToolManager::Get_Instance()->Set_All(1.f);
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
	case STATE_ATTACK:
		m_pModelCom->Set_AnimIndex(21, true);
		break;
	case STATE_SNAPHAT:
		m_pModelCom->Set_AnimIndex(21, true);
		break;
	case STATE_SIT:
		m_pModelCom->Set_AnimIndex(15);
		break;
	case STATE_CANATTACKED:
		m_pModelCom->Set_AnimIndex(8);
		break;
	default:
		break;
	}


}


void CVSnatcher::Compute_Pattern(_float fTimeDelta)
{
	if (1 <= m_iSnatCount)
	{
		m_iSnatCount = 0;
		Set_State(STATE_CANATTACKED);
		return;
	}



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
	_float fOriTime = fTimeDelta;
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
	case STATE_ATTACK:
		Tick_Attack(fTimeDelta);
		break;
	case STATE_CANATTACKED:
		Tick_CanAttacked(fTimeDelta);
		break;
	case STATE_DEAD:
		Tick_Dead(fOriTime);

	case STATE_CUT_1:
		Tick_Cut_1(fTimeDelta);
		break;
	case STATE_CUT_2:
		Tick_Cut_2(fTimeDelta);
		break;
	case STATE_CUT_3:
		Tick_Cut_3(fTimeDelta);
		break;
	case STATE_CUT_4:
		Tick_Cut_4(fTimeDelta);
		break;
	case STATE_CUT_5:
		Tick_Cut_5(fTimeDelta);
		break;
	case STATE_CUT_6:
		Tick_Cut_6(fTimeDelta);
		break;
	case STATE_CUT_7:
		Tick_Cut_7(fTimeDelta);
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
		// TODO Y 소팅!
		//_float fX = CToolManager::Get_Instance()->Get_RendomNum(-4.f, 4.f);
		//_float fZ = CToolManager::Get_Instance()->Get_RendomNum(-4.f, 4.f);
		//vPlayerPos = XMVectorSetX(vPlayerPos, XMVectorGetX(vPlayerPos) + fX);
		//vPlayerPos = XMVectorSetZ(vPlayerPos, XMVectorGetZ(vPlayerPos) + fZ);
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
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 15.f, fTimeDelta);
			m_pCaulDron->Reset_Bound();
			m_fHoItTimeAcc = 0.f;
		}
		else
		{
			vPos = XMVectorSetY(vPos, -10.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));
			
			m_fHoItTimeAcc += fTimeDelta;
			if (0.5f < m_fHoItTimeAcc)
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
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 20.f, fTimeDelta);
			m_fHoItTimeAcc = 0.f;
		}
		else
		{
			vPos = XMVectorSetY(vPos, 0.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));

			if (0.f == m_fHoItTimeAcc && 0 < m_iHoItCount)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				CSplash_wave::SPLASHWAVEDESC Desc;
				XMStoreFloat3(&Desc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Splash_wave"), LEVEL_GAMEPLAY, TEXT("Layer_Skill"), &Desc)))
					return;

				
				CParticleManager::Get_Instance()->Create_Effect(TEXT("Grave1"), _float3(-60.57f, 0.f, -115.45f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(2.f, 2.f, 2.f), _float3(1.f, 1.f, 1.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 90.f), 0.f, 0.f, true, 30.f, 15.f, 2.f,
					40, 20.f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 4.f, 0.2f, _float3(0.f, 0.f, 0.f), _float3(0.f, 360.f, 0.f), CParticle::TYPE_MODLE);
				CCamManager::Get_Instance()->Get_Cam()->Start_Shake(0.2f, 10.f, 0.07f);


				list<CGameObject*>* pCubes = pGameInstance->Get_LayerObjs(LEVEL_BOSS, TEXT("Layer_Cube"));
				if (pCubes)
				{
					for (auto& pCube : *pCubes)
						((CPuzzleCube_Boss*)pCube)->Attacked();
				}



				RELEASE_INSTANCE(CGameInstance);

			}

			m_fHoItTimeAcc += fTimeDelta;
			if (0 < m_iHoItCount)
			{
				if (0.1f > m_fHoItTimeAcc && m_pPlayer->Get_IsPlayerOn())
				{
					if (CPlayer::STATE_DOWN != m_pPlayer->Get_State())
						m_pPlayer->Set_Down();
					else
					{
						((CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")))->ResetGravity();
						((CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")))->Jump(8.f);						
					}
						
				}

				m_pCaulDron->Go_Bound();
			}

			if (0.5f < m_fHoItTimeAcc)
			{
				m_bIsUp = true;
				m_fHoItTimeAcc = 0.f;
				--m_iHoItCount;

				if (0 > m_iHoItCount)
				{
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

void CVSnatcher::Tick_Attack(_float fTimeDelta)
{
	_vector vPlayerPos = ((CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt_ForLandObject(vPlayerPos);

	m_fSnapHatTimeAcc += fTimeDelta;

	if (3.f < m_fSnapHatTimeAcc && 9.f > m_fSnapHatTimeAcc)
	{
		m_pPlayer->Attacked();
		m_fSnapHatTimeAcc = 10.f;
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

		++m_iSnatCount;

		m_fSnapHatTimeAcc = 10.f;

		// CUIManager::Get_Instance()->On_Text(TEXT("유후~"), 0.6f, 1.f, true, true);
	}
	else if (11.5f < m_fSnapHatTimeAcc && 19.f > m_fSnapHatTimeAcc)
	{
		Drop_Hat();
		m_pSockatCom->Remove_Sockat(SLOT_HAND);
		Equip_Sockat(m_sSnapTag, SLOT_HEAD);
		m_fSnapHatTimeAcc = 20.f;

		CUIManager::Get_Instance()->Off_Text(true);
	}
}

void CVSnatcher::Tick_CanAttacked(_float fTimeDelta)
{
	
	if (m_bAttacked)
	{
		m_fAttackedTimeAcc += fTimeDelta;
		if (0.1f < m_fAttackedTimeAcc)
		{
			Set_State(STATE_IDLE);
			m_bAttacked = false;
			m_fAttackedTimeAcc = 0.f;
			m_fCanAttackedTimeAcc = 0.f;
			CToolManager::Get_Instance()->Set_All(1.f);
		}
	}
	else
	{
		m_fCanAttackedTimeAcc += fTimeDelta;

		if (6.5f < m_fCanAttackedTimeAcc)
		{
			Set_State(STATE_IDLE);
			m_bAttacked = false;
			m_fAttackedTimeAcc = 0.f;
			m_fCanAttackedTimeAcc = 0.f;
		}

		
	}

}

void CVSnatcher::Tick_Dead(_float fTimeDelta)
{
	m_fDeadTimeAcc += fTimeDelta;

	if (m_fDeadMaxTimeAcc < m_fDeadTimeAcc && 100 > m_iFinLaserCount)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 4.1f;

		_float3 vTempPos;
		XMStoreFloat3(&vTempPos, vPos);
		vTempPos.y = 2.7f;
		CFinLaser::LASERDESC Desc;
		Desc.vPos = vTempPos;
	
		pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_FinLaser"), LEVEL_BOSS, TEXT("Layer_Ex"), &Desc);

		RELEASE_INSTANCE(CGameInstance);


		++m_iFinLaserCount;
		m_fDeadMaxTimeAcc *= 0.8f;
		m_fDeadTimeAcc = 0.f;

		if (50 < m_iFinLaserCount)
			CUIManager::Get_Instance()->Set_WhiteBoard(true);

	}

	if (2.5f < m_fDeadTimeAcc)
	{
		CToolManager::Get_Instance()->Resul_Level(LEVEL_ENDING);
	}

}



void CVSnatcher::Tick_Cut_1(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_fCutTimeAcc_1 += fTimeDelta;

	if (0 == m_iCutIndex)
	{

		if (1.f < m_fCutTimeAcc_1 && 9.f > m_fCutTimeAcc_1)
		{
			m_pModelCom->Set_AnimIndex(0);
			m_fCutTimeAcc_1 = 10.f;
		}
		else if (11.5f < m_fCutTimeAcc_1)
		{
			if (pGameInstance->Key_Down(DIK_E))
				++m_iTalkCount;

			switch (m_iTalkCount)
			{
			case 0:
				CUIManager::Get_Instance()->On_Text(TEXT("까아아아아아아아아~~~ 꿍!"), 1.f, 2.f, true);
				break;
			case 1:
				CUIManager::Get_Instance()->Set_Text(TEXT(". . . . . ."), 0.8f, 0.5f, true);
				break;
			case 2:
			{
				CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 65.f), Get_PaceLook(5.5f), false);
				m_pModelCom->Set_AnimIndex(26);
				CUIManager::Get_Instance()->Set_Text(TEXT("뭐야"), 0.8f, 0.5f, true);
			}
				break;
			case 3:
				CUIManager::Get_Instance()->Set_Text(TEXT("너는 처음 보는 얼굴인데?"), 0.8f, 0.5f, true);
				break;
			case 4:
				CUIManager::Get_Instance()->Off_Text();
				CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(10.f, 10.f, 30.f), Get_PaceLook(4.5f), false);
				m_pModelCom->Set_AnimIndex(21);
				m_iCutIndex = 1;
				m_fCutTimeAcc_1 = 0.f;
				break;
			default:
				break;
			}
		}
	}
	else if (1 == m_iCutIndex)
	{
		if (pGameInstance->Key_Down(DIK_E))
			++m_iTalkCount;

		if (3.f < m_fCutTimeAcc_1 && 9.f > m_fCutTimeAcc_1)
		{
			Equip_Sockat("TimeObject_Parts", SLOT_HAND);
			m_fCutTimeAcc_1 = 10.f;
		}


		switch (m_iTalkCount)
		{
		case 5:
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, -20.f, 50.f), Get_PaceLook(5.f), false);
			CUIManager::Get_Instance()->Set_Text(TEXT("혹시 이걸 찾고있니?"), 0.8f, 0.5f, true);
			break;
		case 6:
		{
			// CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			// CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 65.f), pSlotGame->Get_TotalPos(), false);
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 65.f), Get_PaceLook(5.5f), false);
			m_pModelCom->Set_AnimIndex(26);
			CUIManager::Get_Instance()->Set_Text(TEXT("뭐? 방금 내가 뺏어갓다고?"), 0.8f, 0.5f, true);
		}
			break;
		case 7:
		{
			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(8.f, 10.f, 65.f), pSlotGame->Get_TotalPos(), false);
			m_pModelCom->Set_AnimIndex(5);
			CUIManager::Get_Instance()->Set_Text(TEXT("하 하 하 하 하 하 하 하 하 하 하 하 하 하 하 하 하!"), 0.8f, 2.f, true);
		}
			break;
		case 8:
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 65.f), Get_PaceLook(5.5f), false);
			m_pModelCom->Set_AnimIndex(26);
			CUIManager::Get_Instance()->Set_Text(TEXT("안타깝지만, 여기에 있는건 전부 내 소유야."), 0.8f, 0.5f, true);
			break;
		case 9:
			CUIManager::Get_Instance()->Set_Text(TEXT("하지만 넌 운이 좋아!"), 0.8f, 0.5f, true);
			break;
		case 10:
			CUIManager::Get_Instance()->Set_Text(TEXT("마침 내가 굉장히 심심했거든."), 0.8f, 0.5f, true);
			break;
		case 11:
			CUIManager::Get_Instance()->Set_Text(TEXT("내가 만족할 만큼 놀아준다면 돌려줄 수도 있지."), 0.8f, 0.5f, true);
			break;
		case 12:
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(10.f, 10.f, 65.f), Get_PaceLook(5.5f), false);
			m_pModelCom->Set_AnimIndex(23);
			CUIManager::Get_Instance()->Set_Text(TEXT("그럼 놀아보자고!!!!"), 1.f, 0.8f, true);
			break;
		default:
			break;
		}
	}


	RELEASE_INSTANCE(CGameInstance);

}

void CVSnatcher::Tick_Cut_2(_float fTimeDelta)
{
	CTransform* pTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
	m_pTransformCom->LookAt_ForLandObject(pTran->Get_State(CTransform::STATE_POSITION));


	m_fCutTimeAcc_1 += fTimeDelta;
	if (0.5f  < m_fCutTimeAcc_1 &&  9.f > m_fCutTimeAcc_1)
	{
		m_pModelCom->Set_AnimIndex(14);
		m_fCutTimeAcc_1 = 10.f;
	}
}

void CVSnatcher::Tick_Cut_3(_float fTimeDelta)
{
	CTransform* pTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
	m_pTransformCom->LookAt_ForLandObject(pTran->Get_State(CTransform::STATE_POSITION));

	if (0 == m_iCutIndex)
		m_iCutIndex = -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (1 == m_iCutIndex)
	{
		if (pGameInstance->Key_Down(DIK_E))
			++m_iTalkCount;

		switch (m_iTalkCount)
		{
		case 0:
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 65.f), Get_PaceLook(5.5f), false);
			m_pModelCom->Set_AnimIndex(26);
			CUIManager::Get_Instance()->On_Text(TEXT("너는 이 이상하게 생긴게 뭔지 아니?"), 0.8f, 0.5f, true);
			break;
		case 1:
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(3.f, -10.f, 30.f), _float3(-28.0f, 1.4f, 44.84f), false);
			m_pModelCom->Set_AnimIndex(26);
			CUIManager::Get_Instance()->Set_Text(TEXT(". . . . . . ."), 0.8f, 0.5f, true);
			break;
		case 2:
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 65.f), Get_PaceLook(5.5f), false);
			m_pModelCom->Set_AnimIndex(26);
			CUIManager::Get_Instance()->Set_Text(TEXT("맞아..."), 0.8f, 0.5f, true);
			break;
		case 3:
			CUIManager::Get_Instance()->Set_Text(TEXT("너무 시끄러운 녀석이여서 내가 눈알만 남겨놨어."), 0.8f, 0.5f, true);
			break;
		case 4:
			CUIManager::Get_Instance()->Set_Text(TEXT("덕분에 아주 묵묵한 좋은 친구가 됐지."), 0.8f, 0.5f, true);
			break;
		case 5:
			CUIManager::Get_Instance()->Set_Text(TEXT("이 친구가 우리 놀이를 도와줄거야."), 0.8f, 0.5f, true);
			break;
		case 6:
			CUIManager::Get_Instance()->Set_Text(TEXT("숲에는 금고가 있어."), 0.8f, 0.5f, true);
			break;
		case 7:
			CUIManager::Get_Instance()->Set_Text(TEXT("금고들을 다 열면 묵묵한 친구가 길을 열어줄거야."), 0.8f, 0.5f, true);
			break;
		case 8:
			CUIManager::Get_Instance()->Set_Text(TEXT("걱정마. 죽으면 너도 눈알로 만들어 줄게."), 0.8f, 1.f, true);
			break;
		case 9:
			m_pModelCom->Set_AnimIndex(9);
			CUIManager::Get_Instance()->Off_Text();
			CCamManager::Get_Instance()->Get_Cam()->Set_State(CCamera_Free::CAM_GAME);
			++m_iTalkCount;
			break;
		default:
			break;
		}
	}


	RELEASE_INSTANCE(CGameInstance);


}

void CVSnatcher::Tick_Cut_4(_float fTimeDelta)
{
	CTransform* pTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
	m_pTransformCom->LookAt_ForLandObject(pTran->Get_State(CTransform::STATE_POSITION));

	if (0 == m_iCutIndex)
		m_iCutIndex = -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (1 == m_iCutIndex)
	{
		if (pGameInstance->Key_Down(DIK_E))
			++m_iTalkCount;

		switch (m_iTalkCount)
		{
		case 0:
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(10.f, 10.f, 30.f), Get_PaceLook(4.5f), false);
			m_pModelCom->Set_AnimIndex(0);
			CUIManager::Get_Instance()->On_Text(TEXT("아아아아주우우우우우우~! 좋아!"), 1.f, 2.f, true);
			break;
		case 1:
		{
			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 30.f), pSlotGame->Get_TotalPos(), false);
			m_pModelCom->Set_AnimIndex(11);
			CUIManager::Get_Instance()->Set_Text(TEXT("너 제법이구나!"), 1.f, 0.5f, true);
		}
			break;
		case 2:
		{
			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 30.f), pSlotGame->Get_TotalPos(), false);
			CUIManager::Get_Instance()->Set_Text(TEXT("그 짧은 다리로 아둥바둥 거리는게 정말 추해!"), 1.f, 0.5f, true);
		}
			break;
		case 3:
		{
			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 30.f), pSlotGame->Get_TotalPos(), false);
			CUIManager::Get_Instance()->Set_Text(TEXT("좋아! 어디 그 콩알만한 뇌도 쓸만한지 볼까?"), 1.f, 0.5f, true);
		}
			break;
		case 4:
		{
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(30.f, 10.f, 65.f), Get_PaceLook(5.5f), false);
			m_pModelCom->Set_AnimIndex(23);
			CUIManager::Get_Instance()->Set_Text(TEXT("쇼 타아아아아아아임~~!!!!!"), 1.5f, 2.f, true);
		}
			break;
		default:
			break;
		}
	}
	else if (2 == m_iCutIndex)
	{
		m_fCutTimeAcc_1 += fTimeDelta;
		if(30.f < m_fCutTimeAcc_1)
			m_pModelCom->Set_AnimIndex(9);
	}


	RELEASE_INSTANCE(CGameInstance);


}

void CVSnatcher::Tick_Cut_5(_float fTimeDelta)
{
	CTransform* pTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
	m_pTransformCom->LookAt_ForLandObject(pTran->Get_State(CTransform::STATE_POSITION));

	if (0 == m_iCutIndex)
	{
		m_pSockatCom->Remove_Sockat(SLOT_HEAD);
		m_pSockatCom->Remove_Sockat(SLOT_HAND);
		m_iCutIndex = -1;
	}


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (2 == m_iCutIndex)
	{
		if (pGameInstance->Key_Down(DIK_E))
			++m_iTalkCount;

		switch (m_iTalkCount)
		{
		case 0:
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(_float3(-68.36f, 1.5f, -60.72f), Get_PaceLook(15.f), false);
			m_pModelCom->Set_AnimIndex(24);
			CUIManager::Get_Instance()->On_Text(TEXT(". . . . . "), 1.f, 2.f, true);
			break;
		case 1:
		{
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(_float3(-68.36f, 1.5f, -60.72f), Get_PaceLook(15.f), false);
			m_pModelCom->Set_AnimIndex(24);
			CUIManager::Get_Instance()->Set_Text(TEXT("우리 잠깐 얘기 좀 할까?"), 1.f, 0.5f, true);
		}
		break;
		case 2:
		{
			m_pModelCom->Set_AnimIndex(21);
			CUIManager::Get_Instance()->Off_Text();
			++m_iTalkCount;
		}
		default:
			break;
		}

		if (3 == m_iTalkCount)
		{
			m_fCutTimeAcc_1 += fTimeDelta;
			if (3.f < m_fCutTimeAcc_1 && 9.f > m_fCutTimeAcc_1)
			{
				m_pPlayer->Set_RenderSkip(true);
				Equip_Sockat("HatKid_statue", SLOT_HAND);
				m_fCutTimeAcc_1 = 10.f;
			}
		}
	}
	else if (3 == m_iCutIndex)
	{
		m_fCutTimeAcc_1 += fTimeDelta;
		if (15.f < m_fCutTimeAcc_1 && 19.f > m_fCutTimeAcc_1)
		{
			// TODO 맵 이동
			CToolManager::Get_Instance()->Resul_Level(LEVEL_BOSS);
			CCamManager::Get_Instance()->Get_Cam()->Set_State(CCamera_Free::CAM_GAME);
			m_pPlayer->Set_RenderSkip(false);
			m_fCutTimeAcc_1 = 20.f;
		}
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 50.f, fTimeDelta);
	}



	RELEASE_INSTANCE(CGameInstance);
}

void CVSnatcher::Tick_Cut_6(_float fTimeDelta)
{
	if (0 == m_iCutIndex)
	{
		CTransform* pTran = (CTransform*)m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));

		_vector vPlayerPos = pTran->Get_State(CTransform::STATE_POSITION);
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vDir = vPlayerPos - vMyPos;
		_vector vNorDir = XMVector3Normalize(vDir);
		_float fDis = XMVectorGetX(XMVector3Length(vDir));

		if (12.f > fDis)
		{
			m_iCutIndex = 2;
			m_bDark = false;
		}
	}




	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (2 == m_iCutIndex)
	{
		if (pGameInstance->Key_Down(DIK_E))
			++m_iTalkCount;

		switch (m_iTalkCount)
		{
		case 0:
			// CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(_float3(-68.36f, 1.5f, -60.72f), Get_PaceLook(15.f), false);
			CUIManager::Get_Instance()->On_Text(TEXT(". . . . . "), 0.8f, 0.5f, true);
			break;
		case 1:
		{
			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			_float3 vPos = pSlotGame->Get_TotalPos();
			vPos.y -= 1.f;
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 70.f), vPos, false);
			CUIManager::Get_Instance()->Set_Text(TEXT("참 고된 하루지...?"), 0.8f, 0.5f, true);
		}
		break;
		case 2:
		{
			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			_float3 vPos = pSlotGame->Get_TotalPos();
			vPos.y -= 1.f;
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 70.f), vPos, false);
			CUIManager::Get_Instance()->Set_Text(TEXT("생각을 해봤는데 말이야."), 0.8f, 0.5f, true);
		}
		break;
		case 3:
		{
			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			_float3 vPos = pSlotGame->Get_TotalPos();
			vPos.y -= 1.f;
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 70.f), vPos, false);
			CUIManager::Get_Instance()->Set_Text(TEXT("내가 너무 못되게 굴었던 것 같아."), 0.8f, 0.5f, true);
		}
		break;
		case 4:
		{
			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			_float3 vPos = pSlotGame->Get_TotalPos();
			vPos.y -= 1.f;
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 70.f), vPos, false);
			CUIManager::Get_Instance()->Set_Text(TEXT("그러니 사과의 의미로..."), 0.8f, 0.5f, true);
		}
		break;
		case 5:
		{
			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			_float3 vPos = pSlotGame->Get_TotalPos();
			vPos.y -= 1.f;
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(7.f, 0.f, 60.f), vPos, false);
			m_pModelCom->Set_AnimIndex(11);
			CUIManager::Get_Instance()->Set_Text(TEXT("가지고 있는 모자 다 내놔!!!!!!!!"), 1.2f, 1.4f, true);
			m_bDark = true;
		}
		break;
		case 6:
		{
			CCamManager::Get_Instance()->Get_Cam()->Set_State(CCamera_Free::CAM_GAME);
			CUIManager::Get_Instance()->Off_Text();
			m_bDark = true;
			++m_iTalkCount;
		}
		break;
		default:
			break;
		}

		if (7 == m_iTalkCount)
		{
			m_fCutTimeAcc_1 += fTimeDelta;
			if (2.f < m_fCutTimeAcc_1 && 9.f > m_fCutTimeAcc_1)
			{
				Set_State(STATE_IDLE);
				End_Dark();
				m_pSockatCom->Remove_Sockat(SLOT_SPIN);
				m_pSockatCom->Remove_Sockat(SLOT_HAND);
				if (FAILED(CDataManager::Get_Instance()->Load_Map(5, LEVEL_BOSS)))   // 3 5
					return;

				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				CGameObject* pObj = nullptr;
				if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_CaulDron"), LEVEL_BOSS, TEXT("Layer_BackGournd"), &pObj, nullptr)))
					return;
				m_pCaulDron = (CCaulDron*)pObj;

				CToilet_Scream::SCREAMDESC Desc;
				Desc.vRotation = _float3(0.f, 0.f, 0.f);
				if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Toilet_Scream"), LEVEL_BOSS, TEXT("Layer_BackGournd"), &Desc)))
					return;
				//Desc.vRotation = _float3(0.f, 45.f, 0.f);
				//if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Toilet_Scream"), LEVEL_BOSS, TEXT("Layer_BackGournd"), &Desc)))
				//	return;

				RELEASE_INSTANCE(CGameInstance);

				m_fCutTimeAcc_1 = 10.f;
			}
		}
	}




	RELEASE_INSTANCE(CGameInstance);
}

void CVSnatcher::Tick_Cut_7(_float fTimeDelta)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (0 == m_iCutIndex)
	{
		if (pGameInstance->Key_Down(DIK_E))
			++m_iTalkCount;

		switch (m_iTalkCount)
		{
		case 0:
		{

			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			_float3 vPos = pSlotGame->Get_TotalPos();
			vPos.y -= 1.f;
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 70.f), vPos, false);
		}
			break;
		case 1:
		{

			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			_float3 vPos = pSlotGame->Get_TotalPos();
			vPos.y -= 1.f;
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 70.f), vPos, false);
			// CUIManager::Get_Instance()->On_Text(TEXT("....."), 0.8f, 0.5f, true);
		}
		break;
		case 2:
		{
			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			_float3 vPos = pSlotGame->Get_TotalPos();
			vPos.y -= 1.f;
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 70.f), vPos, false);
			CUIManager::Get_Instance()->Set_Text(TEXT("......"), 0.8f, 0.5f, true);
		}
		break;
		case 3:
		{
			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			_float3 vPos = pSlotGame->Get_TotalPos();
			vPos.y -= 1.f;
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 70.f), vPos, false);
			CUIManager::Get_Instance()->Set_Text(TEXT("생각보다 근사한 마무리였어."), 0.8f, 0.5f, true);
		}
		break;
		case 4:
		{
			CParts* pSlotGame = (CParts*)m_pSockatCom->Get_SlotPos(SLOT_HEAD);
			_float3 vPos = pSlotGame->Get_TotalPos();
			vPos.y -= 1.f;
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(Get_PacePos(6.f, 10.f, 70.f), vPos, false);
			CUIManager::Get_Instance()->Set_Text(TEXT("그렇지 않니 꼬마야?"), 0.8f, 0.5f, true);
		}
		break;
		case 5:
		{
			CCamManager::Get_Instance()->Get_Cam()->Set_CamFreeValue(_float3(-62.8f, 2.f, -113.f), _float3(-62.6f, 1.29f, -116.f), false);
			m_pModelCom->Set_AnimIndex(26);
			CUIManager::Get_Instance()->Set_Text(TEXT(". . . . . ."), 1.2f, 1.f, true);
		}
		break;
		case 6:
		{
			// 엔딩 로고 끝
		}
		break;
		default:
			break;
		}


	}


	RELEASE_INSTANCE(CGameInstance);
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
	 

	LEVEL eLevel = CToolManager::Get_Instance()->Get_CulLevel();

	// 소켓 갱신
	m_pSockatCom->Tick(fTimeDelta, m_pTransformCom);

	if (LEVEL_ENDING != eLevel)
	{
		if (m_bDark)
			m_pSockatCom->LateTick(fTimeDelta, m_pRendererCom);
		else
			m_pSockatCom->LateTick(fTimeDelta, m_pRendererCom, CRenderer::RENDER_NONLIGHT);

	}
	else
	{
		if(0 < m_iTalkCount)
			m_pSockatCom->LateTick(fTimeDelta, m_pRendererCom, CRenderer::RENDER_NONLIGHT);
	}


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (LEVEL_BOSS == eLevel)
	{
		if (STATE_CANATTACKED == m_eState )
			pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);
	}
	else
		pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);
	

	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f);
	if (true == isDraw)
	{
		if(LEVEL_ENDING != eLevel)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
		else if(0 < m_iTalkCount)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
		}
	}


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
	case STATE_ATTACK:
		Set_State(STATE_IDLE);

	case STATE_CUT_1:
	{
		if (21 == m_pModelCom->Get_CurAnimIndex() && 1 == m_iCutIndex && 4 == m_iTalkCount)
			++m_iTalkCount;
		else if (23 == m_pModelCom->Get_CurAnimIndex() && 1 == m_iCutIndex)
		{
			m_pModelCom->Set_AnimIndex(9);
			CUIManager::Get_Instance()->Off_Text();
			CCamManager::Get_Instance()->Get_Cam()->Set_State(CCamera_Free::CAM_GAME);
			m_iCutIndex = 2;
		}
		else if (9 == m_pModelCom->Get_CurAnimIndex() && 2 == m_iCutIndex)
		{
			Set_State(STATE_CUT_2);
		}
	}
		break;
	case STATE_CUT_2:
	{
		if (9 == m_pModelCom->Get_CurAnimIndex())
			Set_State(STATE_CUT_3);
	}
		break;
	case STATE_CUT_3:
	{
		if (18 == m_pModelCom->Get_CurAnimIndex())
			m_pModelCom->Set_AnimIndex(24);
		else if (9 == m_pModelCom->Get_CurAnimIndex())
			Set_State(STATE_CUT_4);
	}
		break;
	case STATE_CUT_4:
		if (23 == m_pModelCom->Get_CurAnimIndex() && 1 == m_iCutIndex)
		{
			m_iCutIndex = 2;
			CCutSceneManager::Get_Instance()->StartCutScene(CCutSceneManager::CUT_CAM4);
			CUIManager::Get_Instance()->Off_Text();
			m_pModelCom->Set_AnimIndex(11);
		}
		else if (9 == m_pModelCom->Get_CurAnimIndex() && 2 == m_iCutIndex)
			Set_State(STATE_CUT_5);
		break;
	case STATE_CUT_5:
	{
		if (18 == m_pModelCom->Get_CurAnimIndex())
		{
			m_iCutIndex = 2;
			m_pModelCom->Set_AnimIndex(24);
		}
		else if (21 == m_pModelCom->Get_CurAnimIndex())
		{
			// m_pModelCom->Set_AnimIndex(9);
			m_iCutIndex = 3;
		}
	}
		break;
		


	}
}







void CVSnatcher::Create_ExPlo(_fvector vPos)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float3 vTempPos;
	XMStoreFloat3(&vTempPos, vPos);
	vTempPos.y = .4f;
	vTempPos.y += 0.01f * (m_iCurseMaxCount - m_iCurseCount + 1);
	CEyeAttackGround::EYETTACKGROUNDDESC Desc;
	Desc.vPos = vTempPos;

	pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_EyeAttackGround"), LEVEL_BOSS, TEXT("Layer_Ex"), &Desc);

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
		
		fGoTime += 0.3f;
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
	// 모자가 없다 일반 펀치 공격
	if (CItemManager::Get_Instance()->Get_Hats()->empty())
	{
		Set_State(STATE_ATTACK);
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
	m_iSnapIndex = -1;
}

_float3 CVSnatcher::Get_PacePos(_float fLength , _float fUpAngle, _float fRightAngle)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vNorLookDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vNorRightDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	
	_matrix mUp = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fUpAngle));
	_matrix mRight = XMMatrixRotationAxis(-vNorRightDir, XMConvertToRadians(fRightAngle));

	_matrix mTotal = mUp * mRight;

	vNorLookDir = XMVector3Normalize(XMVector3TransformNormal(vNorLookDir, mTotal));

	_float3 vVPos;
	vPos += vNorLookDir * fLength;
	XMStoreFloat3(&vVPos, vPos);
	return vVPos;
}

_float3 CVSnatcher::Get_PaceLook(_float fHight)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fHight);

	// 6 5
	_float3 vVPos;
	XMStoreFloat3(&vVPos, vPos);
	return vVPos;
}

void CVSnatcher::Start_Dark()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Set_Dark(true);
	m_bDark = true;
	m_pPlayer->Set_Dark(true);

	RELEASE_INSTANCE(CGameInstance);
}

void CVSnatcher::End_Dark()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Set_Dark(false);
	m_bDark = false;
	m_pPlayer->Set_Dark(false);

	RELEASE_INSTANCE(CGameInstance);
}


void CVSnatcher::Attacked()
{
	if (m_bAttacked)
		return;

	--m_CreatureDesc.iHP;
	if (0 >= m_CreatureDesc.iHP)
	{
		// 엔딩
		m_bAttacked = true;
		m_CreatureDesc.iHP = 0;
		CToolManager::Get_Instance()->Set_All(0.01f);
		Set_State(STATE_DEAD);
		CCamManager::Get_Instance()->Get_Cam()->Start_Shake(100.f, 2.f, 0.03f);
		CUIManager::Get_Instance()->Off_Text(true);
	}
	else
	{
		// Set_State(STATE_IDLE);
		m_bAttacked = true;
		CToolManager::Get_Instance()->Set_All(0.05f);
		Drop_Hat();
		m_pSockatCom->Remove_Sockat(SLOT_HEAD);

		CUIManager::Get_Instance()->Set_Text(TEXT("*크헉!!!*"), 1.2f, 2.f, true, true);
	}
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

	_bool bBlur = false;


	_uint iIndex = 0;

	if (m_bAttacked)
		iIndex = 7;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (1 == i && m_bDark)
			iIndex = 6;
		else if (0 == i)
			bBlur = true;

		if (FAILED(m_pShaderCom->Set_RawValue("g_bBlur", &bBlur, sizeof(_bool))))
			return E_FAIL;
			
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iIndex)))
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

		if (!strcmp("Attacked_Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
		{
			// Attacked();
		}


		if (!strcmp("Near_Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
		{
			if (STATE_CUT_0 == m_eState)
			{
				Set_State(STATE_CUT_1);
			}
			else if (STATE_CUT_2 == m_eState)
			{
				m_pModelCom->Set_AnimIndex(9);
				// Set_State(STATE_CUT_3);
			}
			else if (STATE_CUT_3 == m_eState && -1 == m_iCutIndex)
			{
				m_iCutIndex = 1;
			}
			else if (STATE_CUT_4 == m_eState && -1 == m_iCutIndex)
			{
				m_iCutIndex = 1;
			}
			else if (STATE_CUT_5 == m_eState && -1 == m_iCutIndex)
			{
				m_iCutIndex = 1;
				m_pModelCom->Set_AnimIndex(18);
			}

		}
		else if (!strcmp("Near_Sphere", Desc.MyDesc.sTag))
		{
			if (STATE_CUT_5 == m_eState && -1 == m_iCutIndex)
			{
				m_iCutIndex = 1;
				m_pModelCom->Set_AnimIndex(18);
			}
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


	ColDesc.vCenter = _float3(0.f, 1.5f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.5f, 2.5f, 2.5f);
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

	if (LEVEL_BOSS == CToolManager::Get_Instance()->Get_CulLevel())
	{
		CGameObject* pObj = nullptr;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Swip"), LEVEL_BOSS, TEXT("Layer_Swip"), &pObj, &WispDesc)))
			return E_FAIL;
		m_pSwip = (CSwip*)pObj;
		Safe_AddRef(m_pSwip);
	}
	else
	{
		CGameObject* pObj = nullptr;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Swip"), LEVEL_GAMEPLAY, TEXT("Layer_Swip"), &pObj, &WispDesc)))
			return E_FAIL;
		m_pSwip = (CSwip*)pObj;
		Safe_AddRef(m_pSwip);
	}

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
	if (FAILED(m_pSockatCom->Match_Enum_BoneName("Spine08", SLOT_SPIN)))
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


	char cItemNameTemp[MAX_PATH];
	strcpy_s(cItemNameTemp, sItemName.data());
	TCHAR cItemName[MAX_PATH];
	CToolManager::Get_Instance()->CtoTC(cItemNameTemp, cItemName);


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
	else if (!lstrcmp(cItemName, TEXT("TimeObject_Parts")))
	{
		if (SLOT_HAND == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("TimeObject_Parts"));
			PartsDesc.vPos = _float3(-3.62, -0.1f, -5.57f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(0.f, 0.f, 0.f);
			PartsDesc.pOwner = this;
		}
	}
	else if (!lstrcmp(cItemName, TEXT("HatKid_statue")))
	{
		if (SLOT_HAND == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("HatKid_statue"));
			PartsDesc.vPos = _float3(-12.42f, -0.56f, -18.34f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(0.f, -16.5f, 0.f);
			PartsDesc.pOwner = this;
		}
	}
	else if (!lstrcmp(cItemName, TEXT("Snatcher_Chair")))
	{
		if (SLOT_SPIN == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Snatcher_Chair"));
			PartsDesc.vPos = _float3(-0.26f, 0.26f, -1.81f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-98.8f, -12.f, -15.9f);
			PartsDesc.pOwner = this;
		}
	}
	else if (!lstrcmp(cItemName, TEXT("Snatcher_Book")))
	{
		if (SLOT_HAND == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("Snatcher_Book"));
			PartsDesc.vPos = _float3(-4.36f, -0.05f, -5.3f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(46.1f, 0.f, 0.f);
			PartsDesc.pOwner = this;
		}
	}
	else if (!lstrcmp(cItemName, TEXT("crown")))
	{
		if (SLOT_HEAD == eSlot)
		{
			lstrcpy(PartsDesc.m_szModelName, TEXT("crown"));
			PartsDesc.vPos = _float3(0.22f, 0.f, -6.61f);
			PartsDesc.vScale = _float3(1.f, 1.f, 1.f);
			PartsDesc.vRot = _float3(-90.f, -28.f, 0.f);
			PartsDesc.pOwner = this;
		}
	}

	

	if (!FAILED(m_pSockatCom->Add_Sockat(eSlot, m_pModelCom, TEXT("Prototype_GameObject_Parts"), PartsDesc)))
		return E_FAIL;

	return S_OK;
}





CVSnatcher* CVSnatcher::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVSnatcher*		pInstance = new CVSnatcher(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVSnatcher"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVSnatcher::Clone(void * pArg)
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
