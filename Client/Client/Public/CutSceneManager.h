#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CCutSceneManager final : public CBase
{
	DECLARE_SINGLETON(CCutSceneManager)

public:
	enum CUT_STATE { CUT_CAM3, CUT_END };

private:
	CCutSceneManager();
	virtual ~CCutSceneManager() = default;

public:
	void Tick(_float fTimeDelta);

	void StartCutScene(CUT_STATE eState);

	// Cam_3
public:
	void Tick_Cam3(_float fTimeDelta);

private:
	CUT_STATE m_eState = CUT_END; 
	_float m_fCam3_SlowTimeAcc = 0.f;



public:
	virtual void Free() override;

};

END