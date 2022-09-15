#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CCamManager final : public CBase
{
	DECLARE_SINGLETON(CCamManager)

public:
	typedef struct CamData{

		_float3 vPos;
		_float fSpeedAndStopTime;

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

	const list<class CColorCube*>* Get_MarkCubes() { return &m_MarkCubes; }

public:
	void Create_CamTool();
	void Create_SelectingCube();

	void Create_MarkCube();
	void Delete_MarkCube();

	void Create_RenderPosCube(_float3 vPos);
	
	void Create_LookCube();
	void Delete_LookCube();
	


public:
	void Tick(_float fTimeDelta);

	void Input(_float fTimeDelta);


public: // .For CutScene
	void PlayCutScene(_float fTimeDelta);
	void MakePos();
	_float3 CalculBasi();




	void MakeRenderPos();
	void RenderPos(_int iSens);

	void CalculRenderBasi(list<CAMDATA> Poss);
	void GetBesierRenderPos(CAMDATA vPos1);
	void GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2);
	void GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2, CAMDATA vPos3);
	void GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2, CAMDATA vPos3, CAMDATA vPos4);

private:
	class CCamera_CamTool* m_pCamTool = nullptr;
	class CCamSelectingCube* m_pSelectingCube = nullptr;
	list<class CColorCube*> m_MarkCubes;
	list<class CColorCube*> m_TempMarkCubes;
	list<class CColorCube*> m_pTempBasiMarkCubes;

	string m_sSelectedMarkCubeTag;

	_uint m_iTagIndex = 0;
	
	_bool m_bStart = false;
	_float m_fSpeed = 0.f;
	_float m_fTimeAcc = 0.f;
	_float m_fMoveSens = 0.f;
	_float m_fT = 0.f;

	list<CAMDATA> m_PlayPosTemp;
	list<class CRenderCube*> m_RenderCubes;
	_float3 m_vTempPos;


public:
	virtual void Free() override;

};

END

