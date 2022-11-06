#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CCutSceneManager final : public CBase
{
	DECLARE_SINGLETON(CCutSceneManager)

public:
	enum CUT_STATE { CUT_CAM3, CUT_EYE, CUT_END };

private:
	CCutSceneManager();
	virtual ~CCutSceneManager() = default;

	// Common
public:
	void Tick(_float fTimeDelta);

	void StartCutScene(CUT_STATE eState);
	CUT_STATE Get_CurCutScene() { return m_eState; }


	void Add_Obj(class CGameObject* pObj, _uint iIndex);
	void Clear_Obj();

private:
	CUT_STATE m_eState = CUT_END;
	class CGameObject* m_pObjs[100];

	// Cam_3
public:
	void Tick_Cam3(_float fTimeDelta);

private: 
	_float m_fCam3_SlowTimeAcc = 0.f;
	

	// DisApear SubEye
public:
	void Tick_Eye(_float fTimeDelta);

private:
	_float m_fEndTimeAcc = 0.f;
	_float m_fDisAppearEyeTimeAcc = 0.f;


public:
	virtual void Free() override;

};

END