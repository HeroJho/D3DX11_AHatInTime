#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "DataManager.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CMarkCube final : public CGameObject
{
public:
	typedef struct tagColorCubeDesc
	{
		_float4 vColor;
		_float3 vPos;
		_float3 vScale;
	}COLORCUBEDESC;

	typedef struct tagCamData
	{
		_float3 vPos;
		_float fSpeed;
		_uint iLinkIndex;
	}CAMDATA;

private:
	CMarkCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMarkCube(const CMarkCube& rhs);
	virtual ~CMarkCube() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Pos(_float3 vPos);
	_float3 Get_Pos();

	void Set_LinkIndex(_int iIndex) { m_iLinkIndex = iIndex; }
	_int Get_LinkIndex() { return m_iLinkIndex; }

	void Set_SpeedTime(_float fSpeedTime) {
		if (0.001f > fSpeedTime) m_fSpeedTime = 1.0f;
		else m_fSpeedTime = fSpeedTime;
	}
	_float Get_SpeedTime() { return m_fSpeedTime; }


	CAMDATA Get_SaveDATA();
	void Set_SaveDATA(CDataManager::CAMDATA* pData);

private:
	CTransform*				m_pTransformCom = nullptr;

private:
	_int m_iLinkIndex = 0;
	_float m_fSpeedTime = 0.f;


private:
	HRESULT SetUp_Components();



public:
	static CMarkCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END