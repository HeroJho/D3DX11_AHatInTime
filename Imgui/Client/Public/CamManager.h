#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CCamManager final : public CBase
{
	DECLARE_SINGLETON(CCamManager)

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


public:
	void Tick(_float fTimeDelta);

	void Input(_float fTimeDelta);


public: // .For CutScene
	void PlayCutScene(_float fTimeDelta);
	void MakePos();
	void RenderPos(_int iSens);
	void MakeRenderPosCube(_float3 vPos);

	void CalculBasi(list<_float3> Poss);
	void GetBesierPos(_float3 vPos1);
	void GetBesierPos(_float3 vPos1, _float3 vPos2);
	void GetBesierPos(_float3 vPos1, _float3 vPos2, _float3 vPos3);
	void GetBesierPos(_float3 vPos1, _float3 vPos2, _float3 vPos3, _float3 vPos4);

private:
	class CCamera_CamTool* m_pCamTool = nullptr;
	class CCamSelectingCube* m_pSelectingCube = nullptr;
	list<class CColorCube*> m_MarkCubes;

	string m_sSelectedMarkCubeTag;

	_uint m_iTagIndex = 0;
	


	list<_float3> m_PlayPosTemp;
	list<class CColorCube*> m_RenderCubes;

	_bool m_bStart = false;
	_bool m_bDest = false;
	_float m_fTimeAcc = 0.f;
	_float m_fZeroToOneAcc = 0.f;

	_float m_fMoveSens = 0.f;



public:
	virtual void Free() override;

};

END

