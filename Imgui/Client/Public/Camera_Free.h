#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	enum CAM_TYPE { CAM_TOOL, CAM_GAME, CAM_END };

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
	void ChangeGameMode(CGameObject* pPlayer);
	void ChangeToolMode();
	
	CTransform* Get_Transform() { return m_pTransformCom; }


private:
	void Tool_Mode(_float fTimeDelta);

	void Game_Mode(_float fTimeDelta);
	void Game_Mode_Input(_float fTimeDelta);

private:
	CAM_TYPE m_eType = CAM_TOOL;
	CGameObject*		m_pPlayer = nullptr;
	_float3				m_vAngle;
	_float				m_fDis = 0.f;

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END