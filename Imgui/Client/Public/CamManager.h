#pragma once
#include "Client_Defines.h"
#include "Base.h"

#include "DataManager.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CCamManager final : public CBase
{
	DECLARE_SINGLETON(CCamManager)

public:
	typedef struct CamData{

		_float3 vPos;
		_float fSpeedAndStopTime;
		_bool bMark;

	}CAMDATA;

public:
	CCamManager();
	virtual ~CCamManager() = default;

public:
	_float3 Get_SelectingCubePos();
	void Set_SelectingCube(_float3 vPos);

	string Get_SelectedMarkCubeTag() { return m_sSelectedMarkCubeTag; }
	void Set_SelectedMarkCubeTag(string sSelectedMarkCubeTag) { m_sSelectedMarkCubeTag = sSelectedMarkCubeTag;}

	_bool Get_Start() { return m_bStart; }
	void Set_Start(_bool bStart);

	_bool Get_ShowCube() { return m_bShowCube; }
	void Set_ShowCube(_bool bShowCube) { m_bShowCube = bShowCube; }

	_uint Get_Index() { return m_iSaveIndex; }
	void Set_Index(_uint iSaveIndex) { m_iSaveIndex = iSaveIndex; }



	const list<class CMarkCube*>* Get_MarkCubes() { return &m_MarkCubes; }
	const list<class CLookCube*>* Get_LookCubes() { return &m_LookCubes; }

public:
	// For. Common
	void Create_CamTool();
	void Create_SelectingCube();
	void Set_SelectingCubePosToSelectedCube();
	CGameObject* Get_SelectedCube();

	_bool Check_Exception();

	void Clear_PosData();
	void Clear_LookData();
	void Clear_Data();

	// For. MarkCube
	void Create_MarkCube(CDataManager::CAMDATA* pData = nullptr);
	void Delete_MarkCube();
	// For. RenderCube
	void Create_RenderPosCube(_float3 vPos, _bool vMark);
	
public:
	// For. LookCube
	void Create_ChaseLookCube();

	void Create_LookCube(CDataManager::CAMDATA* pData = nullptr);
	void Delete_LookCube();





public:
	void Tick(_float fTimeDelta);

	void Input(_float fTimeDelta);


public: // .For CutScene
	void PlayCutScene(_float fTimeDelta);

	void PlayMark(_float fTimeDelta);
	void PlayLook(_float fTimeDelta);

	void MakeMarkPos();
	void MakeLookPos();

	_float3 CalculMarkBasi();
	_float3 CalculLookBasi();


	// For. RenderCutScene
	void MakeRenderPos();
	void MakeMarkRenderPos();
	void MakeLookRenderPos();
	void RenderPos(_int iSens);

	void CalculRenderBasi(list<CAMDATA> Poss);

	void GetBesierRenderPos(CAMDATA vPos1);
	void GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2);
	void GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2, CAMDATA vPos3);
	void GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2, CAMDATA vPos3, CAMDATA vPos4);


	void Save_Data();
	void Load_Data();


private:
	// For. Common
	class CCamera_CamTool* m_pCamTool = nullptr;
	class CCamSelectingCube* m_pSelectingCube = nullptr;
	class CColorCube* m_pChaseLookCube = nullptr;
	string m_sSelectedMarkCubeTag;
	_uint m_iTagIndex = 0;
	_bool m_bShowCube = false;

	_uint m_iSaveIndex = 0;


	// For. Play
	list<class CMarkCube*> m_MarkCubes;
	list<class CMarkCube*> m_TempMarkCubes;
	list<class CMarkCube*> m_pTempBasiMarkCubes;

	list<class CLookCube*> m_LookCubes;
	list<class CLookCube*> m_TempLookCubes;
	list<class CLookCube*> m_pTempBasiLookCubes;

	_bool m_bStart = false;
	_float m_fMarkSpeed = 0.f;
	_float m_fMarkTimeAcc = 0.f;
	_float m_fMarkMoveSens = 0.f;
	_float m_fMarkT = 0.f;
	_float3 m_vMarkTempPos;

	_float m_fLookSpeed = 0.f;
	_float m_fLookTimeAcc = 0.f;
	_float m_fLookMoveSens = 0.f;
	_float m_fLookT = 0.f;
	_float3 m_vLookTempPos;

	// For. Render
	list<CAMDATA> m_PlayPosTemp;
	list<class CRenderCube*> m_RenderCubes;



public:
	virtual void Free() override;

};

END

