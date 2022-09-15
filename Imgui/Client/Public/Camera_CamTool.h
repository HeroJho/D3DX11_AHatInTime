#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_CamTool final : public CCamera
{
public:
	typedef struct tagRecordCamData
	{
		_float3 vPos;
		// _float3 vLook;

	}RECORDCAMDATA;

private:
	CCamera_CamTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_CamTool(const CCamera_CamTool& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CCamera_CamTool() = default;

public:
	_vector Get_Pos();
	void Set_Pos(_float3 vPos);
	_bool Move(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimitDistance = 0.1f);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Input(_float fTimeDelta);
	
	//void Record(_float fTimeDelta);

public:
	static CCamera_CamTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END