
#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "DataManager.h"

#include "Camera_Free.h"
#include "Terrain.h"
#include "Player.h"
#include "StaticModel_Col.h"
#include "StaticModel_Instance.h"
#include "SkyModel.h"

#include "Mad_Crow.h"
#include "Vault_Mad_Crow.h"
#include "SubSpider.h"
#include "StatuePosed_Boss.h"

#include "Parts.h"
#include "Umbrella.h"
#include "Yarn.h"
#include "Flask.h"
#include "Vault.h"
#include "Vault_2.h"
#include "Vault_State.h"
#include "Diamond.h"
#include "MonsterVault.h"
#include "SubconEye.h"
#include "Wisp.h"
#include "BellMount.h"
#include "BellMountEye.h"
#include "PuzzleCube.h"
#include "RotateBarrel.h"
#include "Splash_wave.h"

#include "StatuePosed.h"
#include "BadgeS_Base.h"
#include "IceBox.h"
#include "TimeObject.h"
#include "Swip.h"
#include "CaulDron.h"

#include "SwipsSky.h"
#include "SwipsSky_Boss.h"

#include "VSnatcher.h"
#include "ExPlo.h"
#include "Magic.h"
#include "Hat.h"
#include "PuzzleCube_Boss.h"
#include "Toilet_Scream.h"
#include "Particle.h"
#include "FlaskLight.h"
#include "WitchChargEffect.h"
#include "Wind_Spiral.h"
#include "SubconBossEye.h"

#include "UI_Edit.h"
#include "UI_Edit_Button.h"
#include "UI_Health.h"
#include "UI_TextLife.h"

#include "UI_Inven.h"
#include "UI_ItemButton.h"
#include "UI_Item_Inven.h"
#include "UI_Item_Inven_Slot.h"
#include "UI_Number.h"
#include "UI_Diamond.h"
#include "UI_X.h"
#include "UI_DiamondScore.h"

#include "UI_Shop.h"
#include "UI_Shop_Slot.h"
#include "UI_Shop_SlotItem.h"
#include "UI_ShopMenu.h"
#include "UI_SpeechBubble.h"
#include "UI_SmallSpeechBubble.h"
#include "WhiteBoard.h"

#include "RollingBarrel.h"
#include "RectBarrel.h"
#include "SpikeBlock.h"

#include "ColorCube.h"
#include "MarkCube.h"
#include "LookCube.h"


#include "Flask_EX.h"
#include "Toilet_SentorWall.h"

#include "MushRoom.h"
#include "Bindi.h"


#include "EyeAttackGround.h"
#include "FinLaser.h"
#include "Fire.h"

#include "Test.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CS());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	case LEVEL_BOSS:
		pLoader->Loading_ForBossLevel();
		break;
	case LEVEL_ENDING:
		pLoader->Loading_ForEndingLevel();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CS());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));
	/* 객체원형 로드한다. */	

	// TEST
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Edit"),
		CUI_Edit::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Edit_Button */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Edit_Button"),
		CUI_Edit_Button::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */


	/* For.Prototype_Component_Texture_Default */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	// TEST
	/* For.Prototype_Component_Texture_InvenFrame */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/TestUI/Inventory/InvenFrame.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_InvenBtn */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenBtn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/TestUI/Inventory/InvenBtn/CashBtn%d.png"), 2))))
		return E_FAIL;







	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));	
	/* 모델를 로드한다. */







	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));

	/* 개ㅑㄱ체원형 로드한다. */
	/* For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), 
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	/* For.Prototype_GameObject_Mad_Crow */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mad_Crow"),
		CMad_Crow::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Vault_Mad_Crow */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Vault_Mad_Crow"),
		CVault_Mad_Crow::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Spider */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SubSpider"),
		CSubSpider::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_SubconEye */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SubconEye"),
		CSubconEye::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_BellMount */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BellMount"),
		CBellMount::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_BellMountEye */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BellMountEye"),
		CBellMountEye::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_StatuePosed_Boss */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StatuePosed_Boss"),
		CStatuePosed_Boss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Splash_wave */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Splash_wave"),
		CSplash_wave::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CCaulDron */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CaulDron"),
		CCaulDron::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_FlaskLight */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FlaskLight"),
		CFlaskLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_SubconBossEye */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SubconBossEye"),
		CSubconBossEye::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_SwipsSky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SwipsSky"),
		CSwipsSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_SwipsSky_Boss */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SwipsSky_Boss"),
		CSwipsSky_Boss::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Bindi*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bindi"),
		CBindi::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PuzzleCube */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PuzzleCube"),
		CPuzzleCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_PuzzleCube_Boss */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PuzzleCube_Boss"),
		CPuzzleCube_Boss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Particle */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle"),
		CParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Fire */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fire"),
		CFire::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	
	/* For.Prototype_GameObject_RollingBarrel */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RollingBarrel"),
		CRollingBarrel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_RectBarrel */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RectBarrel"),
		CRectBarrel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_RotateBarrel */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotateBarrel"),
		CRotateBarrel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Wisp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wisp"),
		CWisp::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Swip */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Swip"),
		CSwip::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Toilet_SentorWall */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Toilet_SentorWall"),
		CToilet_SentorWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WitchChargEffect */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WitchChargEffect"),
		CWitchChargEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_MushRoom */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MushRoom"),
		CMushRoom::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_WhiteBoard */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WhiteBoard"),
		CWhiteBoard::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	
	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain_StaticModel*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticModel"),
		CStaticModel_Col::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* 	Prototype_GameObject_StaticModel_Instance*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticModel_Instance"),
		CStaticModel_Instance::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSkyModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_Parts*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Parts"),
		CParts::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Umbrella*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Umbrella"),
		CUmbrella::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Yarn*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yarn"),
		CYarn::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Hat*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hat"),
		CHat::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Flask*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Flask"),
		CFlask::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Flask_EX*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Flask_EX"),
		CFlask_EX::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Vault*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Vault"),
		CVault::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Vault*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Vault_2"),
		CVault_2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_MonsterVault*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterVault"),
		CMonsterVault::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Vault_State*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Vault_State"),
		CVault_State::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Diamond*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Diamond"),
		CDiamond::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_SpikeBlock*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpikeBlock"),
		CSpikeBlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BadgeS_Base*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BadgeS_Base"),
		CBadgeS_Base::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_StatuePosed*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StatuePosed"),
		CStatuePosed::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_IceBox*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IceBox"),
		CIceBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_TimeObject*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TimeObject"),
		CTimeObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_VSnatcher*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VSnatcher"),
		CVSnatcher::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Toilet_Scream*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Toilet_Scream"),
		CToilet_Scream::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	
	/* For.Prototype_GameObject_ColorCube*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ColorCube"),
		CColorCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_MarkCube*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MarkCube"),
		CMarkCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_LookCube*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LookCube"),
		CLookCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_EyeAttackGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EyeAttackGround"),
		CEyeAttackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Wind_Spiral*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wind_Spiral"),
		CWind_Spiral::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_FinLaser*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FinLaser"),
		CFinLaser::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_UI_Health*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Health"),
		CUI_Health::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_UI_TextLife*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_TextLife"),
		CUI_TextLife::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_UI_Number*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Number"),
		CUI_Number::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_UI_TextLife*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_DiamondScore"),
		CUI_DiamondScore::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_UI_TextLife*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Diamond"),
		CUI_Diamond::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_UI_TextLife*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_X"),
		CUI_X::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_UI_Shop*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Shop"),
		CUI_Shop::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_UI_Shop*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Shop_Slot"),
		CUI_Shop_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_UI_Shop*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Shop_SlotItem"),
		CUI_Shop_SlotItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_UI_ShopMenu*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_ShopMenu"),
		CUI_ShopMenu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_UI_SpeechBubble*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_SpeechBubble"),
		CUI_SpeechBubble::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_UI_SmallSpeechBubble*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_SmallSpeechBubbles"),
		CUI_SmallSpeechBubble::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	

	/* For.Prototype_UI_TextLife*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Test"),
		CTest::Create(m_pDevice, m_pContext))))
		return E_FAIL;





	// TEST
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Inven"),
		CUI_Inven::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_ItemButton"),
		CUI_ItemButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Item_Inven"),
		CUI_Item_Inven::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Item_Inven_Slot"),
		CUI_Item_Inven_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Grass */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Meshes/NonAnim/SubCon/Grass.dds"), 1))))
		return E_FAIL;



	// =========== Particle ====================

	/* For.Prototype_Component_Texture_T_FX_Flare_01 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_FX_Flare_01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/T_FX_Flare_01.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Star */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/Star_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_star_shuriken */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_star_shuriken"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/star_shuriken.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_distortionclouds_tex*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_distortionclouds_tex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/distortionclouds_tex.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Fire_D*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Fire_D"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/Fire_D.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Fire_M*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Fire_M"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/Fire_M.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Fire_Noise*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Fire_Noise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/Fire_Noise.dds"), 1))))
		return E_FAIL;


	// ==================== UI=====================
	if (FAILED(Loading_UI()))
		return E_FAIL;

	if (FAILED(Loading_ObjTexture()))
		return E_FAIL;




	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* 모델를 로드한다. */

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Map_Terrain"),
		CVIBuffer_Map_Terrain::Create(m_pDevice, m_pContext, 2, 2))))
		return E_FAIL;

	/* For.Prototype_Component_Sky */
	_matrix mPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sky"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/SubconSky/", "SubconSky.fbx", mPivot))))
		return E_FAIL;


	/* For. NoneAnimModel */
	if (FAILED(Loading_Model_NoneAnim()))
		return E_FAIL;


	/* For. AnimModel */
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("HatGirl"), LEVEL_STATIC, CDataManager::DATA_ANIM);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("Mad_Crow"), LEVEL_GAMEPLAY, CDataManager::DATA_ANIM);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("SubSpider"), LEVEL_GAMEPLAY, CDataManager::DATA_ANIM);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("vault"), LEVEL_GAMEPLAY, CDataManager::DATA_ANIM);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("SubconEye"), LEVEL_GAMEPLAY, CDataManager::DATA_ANIM);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("BellMount"), LEVEL_GAMEPLAY, CDataManager::DATA_ANIM);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("BadgeS_Base"), LEVEL_GAMEPLAY, CDataManager::DATA_ANIM);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("StatuePosed"), LEVEL_GAMEPLAY, CDataManager::DATA_ANIM);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("TimeObject"), LEVEL_GAMEPLAY, CDataManager::DATA_ANIM);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("VSnatcher"), LEVEL_GAMEPLAY, CDataManager::DATA_ANIM);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("cauldron"), LEVEL_GAMEPLAY, CDataManager::DATA_ANIM);

	/* For. PartsModel */
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("Ori_Hat"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("Sprint_Hat"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("Witch_Hat"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("Mask_Cat"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("Mask_Fox"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("Umbrella"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);

	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("yarn_ui_brew"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("yarn_ui_hover"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("yarn_ui_ice"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("yarn_ui_sprint"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("science_owlbrew_remade"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("capsule"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("TimeObject_Parts"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("HatKid_statue"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("Snatcher_Book"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("Snatcher_Chair"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);
	CDataManager::Get_Instance()->Create_Try_BinModel(TEXT("crown"), LEVEL_GAMEPLAY, CDataManager::DATA_PARTS);


	

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));

	/* For.Prototype_Component_Shader_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Terrain"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Model */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Model_Instance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model_Instance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel_Instance.hlsl"), VTXMODELINSTANCE_DECLARATION::Elements, VTXMODELINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));

	vector<CCell*> Cells = CDataManager::Get_Instance()->Load_Navi(3); // 3 5

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, &Cells))))
		return E_FAIL;

	if (FAILED(CToolManager::Get_Instance()->Clone_Navi()))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForBossLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));
	/* 개ㅑㄱ체원형 로드한다. */

	/* For.Prototype_GameObject_ExPlo*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExPlo"),
		CExPlo::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Magic*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Magic"),
		CMagic::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */










	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* 모델를 로드한다. */










	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));








	lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));

	vector<CCell*> Cells = CDataManager::Get_Instance()->Load_Navi(5); // 3 5

																	   /* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, &Cells))))
		return E_FAIL;

	if (FAILED(CToolManager::Get_Instance()->Clone_Navi(LEVEL_BOSS)))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForEndingLevel()
{
	
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));
	/* 개ㅑㄱ체원형 로드한다. */





	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */










	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* 모델를 로드한다. */










	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));








	lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));

	vector<CCell*> Cells = CDataManager::Get_Instance()->Load_Navi(5); // 3 5

																		/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, &Cells))))
		return E_FAIL;

	if (FAILED(CToolManager::Get_Instance()->Clone_Navi(LEVEL_ENDING)))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;

}


HRESULT CLoader::Loading_Model_NoneAnim()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



	const list<string>* FileNames = CDataManager::Get_Instance()->Get_NonAnimFileNames();

	_matrix		PivotMatrix = XMMatrixIdentity();
	for (auto& sFileName : *FileNames)
	{
		_tchar czTag[MAX_PATH];
		_tchar* cTag = CToolManager::Get_Instance()->Get_ManagedTChar();
		CToolManager::Get_Instance()->CtoTC(sFileName.data(), czTag);

		memcpy(cTag, czTag, sizeof(_tchar) * MAX_PATH);

		CDataManager::Get_Instance()->Create_Try_BinModel(cTag, LEVEL_GAMEPLAY, CDataManager::DATA_NOEANIM);
	}




	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_UI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_Component_Texture_PlayerHealth */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerHealth"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerHealth/playerhealth_new_%d.dds"), 5))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_LifeText */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LifeText"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerHealth/text_life.dds"), 1))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_SmartEye */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SmartEye"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Meshes/Anim/HatGirl/hatkid_smarteyes.dds"), 1))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_Icon_KidHat */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_KidHat"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inven/itemicon_kidhat.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_SprintHat */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_SprintHat"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inven/sprint_hat.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_WitchHat */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_WitchHat"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inven/witch_hat.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_DwellerMask */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DwellerMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inven/DwellerMask_Flair_1.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Foxmask */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Foxmask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inven/itemicon_foxmask.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Selector */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Selector"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inven/Selector.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Icon_SprintYarn */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_SprintYarn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inven/yarn_ui_sprint.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_WitchYarn */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_WitchYarn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inven/yarn_ui_brew.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_IceYarn */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_IceYarn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inven/yarn_ui_ice.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Number */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Number"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Number/number%d.dds"), 10))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_EnergyBit */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_EnergyBit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EnergyBit.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_X */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_X"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/locked_screen_x.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ShopWindow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopWindow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/ShopWindow.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_ShopWindowItem */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopWindowItem"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/ShopWindowItem.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_ShopMenu_costbg */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopMenu_costbg"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/ShopMenu_costbg.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_shop_stand */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_shop_stand"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/shop_stand_%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_speech_bubble */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_speech_bubble"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/speech_bubble_gloom.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_smalltalk */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_smalltalk"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/smalltalk.dds"), 1))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ObjTexture()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_Component_Texture_PipeScream_faceloop */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PipeScream_faceloop"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Object/Toilet_Scream/PipeScream_faceloop.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_T_Noise01 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_Noise01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Object/Toilet_Scream/T_Noise01.dds"), 1))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	CloseHandle(m_hThread);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);


}
