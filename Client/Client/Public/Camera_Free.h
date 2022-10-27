#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	enum CAM_STATE { CAM_GAME, CAM_CUTSCENE, CAM_END };

private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Pos(_float3 vPos);

	CAM_STATE Get_State() { return m_eState; }
	void Set_State(CAM_STATE eState) { m_eState = eState; }

public:
	_bool Move(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimitDistance = 0.1f);
	void LookAt(_fvector vTargetPos);


private:
	void Game_Mode(_float fTimeDelta);
	void Game_Mode_Input(_float fTimeDelta);

	void CutScene_Mode(_float fTimeDelta);

private:
	CGameObject*		m_pPlayer = nullptr;
	_float3				m_vAngle;
	_float				m_fDis = 0.f;

	CAM_STATE			m_eState = CAM_GAME;

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END