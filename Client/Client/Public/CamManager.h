#pragma once
#include "Client_Defines.h"
#include "Base.h"

#include "DataManager.h"


BEGIN(Engine)

class CGameObject;
class CTransform;

END

BEGIN(Client)

class CCamManager final : public CBase
{
	DECLARE_SINGLETON(CCamManager)

public:
	typedef struct CamData {

		_float3 vPos;
		_float fSpeedAndStopTime;
		_bool bMark;

	}CAMDATA;

public:
	CCamManager();
	virtual ~CCamManager() = default;


public:
	void Tick(_float fTimeDelta);




	// For. GameCam
public:
	HRESULT Create_Cam();
	
	class CCamera_Free* Get_Cam() { return m_pCamTool; }



private:







	// For. CutScene
public:
	HRESULT Init();

public:
	_bool Get_Start() { return m_bStart; }
	void Set_Start(_bool bStart);

	_bool Get_IsEnd() { return !m_bStart; }

public:
	void Play_CutScene(_uint iIndex, _bool bAutoEnd, class CTransform* pTran = nullptr);
	void End_CutScene();


public:
	// For. Common
	void Clear_PosData();
	void Clear_LookData();
	void Clear_Data();

	// For. MarkCube
	void Create_MarkCube(CDataManager::CAMDATA* pData = nullptr);


public:
	// For. LookCube
	void Create_ChaseLookCube();

	void Create_LookCube(CDataManager::CAMDATA* pData = nullptr);


public: 
	void PlayCutScene(_float fTimeDelta);

	void PlayMark(_float fTimeDelta);
	void PlayLook(_float fTimeDelta);

	void MakeMarkPos();
	void MakeLookPos();

	_float3 CalculMarkBasi();
	_float3 CalculLookBasi();



	void Load_Data();


private:
	// For. Common
	class CCamera_Free* m_pCamTool = nullptr;
	class CColorCube* m_pChaseLookCube = nullptr;


	// For. Play
	list<class CMarkCube*> m_MarkCubes;
	list<class CMarkCube*> m_TempMarkCubes;
	list<class CMarkCube*> m_pTempBasiMarkCubes;

	list<class CLookCube*> m_LookCubes;
	list<class CLookCube*> m_TempLookCubes;
	list<class CLookCube*> m_pTempBasiLookCubes;

	_bool m_bStart = false;
	_bool m_bAutoEnd = false;
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

	_uint m_iCurCutIndex = 0;

private:
	vector<CDataManager::DATA_CAMS*> m_Datas;


public:
	virtual void Free() override;

};

END

