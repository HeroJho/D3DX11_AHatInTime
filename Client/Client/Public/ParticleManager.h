#pragma once
#include "Client_Defines.h"
#include "Base.h"

#include "Particle.h"


BEGIN(Client)

class CParticleManager final : public CBase
{
	DECLARE_SINGLETON(CParticleManager)

public:
	typedef struct ParticleToolDesc
	{
		_uint iID;
		CParticle::PARTICLEDESC Desc;

		_uint iCount;
		_float fRandPos;
		_float fRandScale;
		_float fRandScaleSpeed;
		_float vRandRotation;
		_float fRandStopValue;
		_float fRandSpeed;
		_float fRandGravityValue;
		_float fRandJumpPower;
		_float fRandLifeTime;

		_float3 vLimitAngleMin;
		_float3 vLimitAngleMax;

	}PARTICLETOOLDESC;



public:
	CParticleManager();
	virtual ~CParticleManager() = default;

	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);




	// For. Client
public:
	/*�� ��, �θ� ����, �θ� ��, ���� ����, ���� ��, ������, ������ ���� ��, ȸ��, x ����, x ���� ��, x �ӵ�, �߷� ����, �߷� ���� ��, ���� �Ŀ�, ������ Ÿ�� */
	void Create_Particle(const TCHAR* cModelTag, _float3 vParentPos, _float3 vPos, _float3 vLook, _float3 vScale, _float3 vScaleSpeed, _float3 vRotaion, _float3 vDir, _float fStopValue, _float fSpeed, _bool bGravity, _float fGravityValue, _float fJumpPower, _float fLifeTime, CParticle::PARTICLE_TYPE eType);

	/*�� ��, �θ� ����, �θ� ��, ���� ����, ���� ��, ������, ������ ���� ��, ȸ��, x ����, x ���� ��, x �ӵ�, �߷� ����, �߷� ���� ��, ���� �Ŀ�, ������ Ÿ�� */
	void Create_Effect(const TCHAR* cModelTag, _float3 vParentPos, _float3 vPos, _float3 vLook, _float3 vScale, _float3 vScaleSpeed, _float3 vRotaion, _float3 vDir, _float fStopValue, _float fSpeed, _bool bGravity, _float fGravityValue, _float fJumpPower, _float fLifeTime,
		_uint iCount, _float fRandPos, _float fRandScale, _float fRandScaleSpeed, _float vRandRotation, _float fRandStopValue, _float fRandSpeed, _float fRandGravityValue, _float fRandJumpPower, _float fRandLifeTime, _float3 vLimitAngleMin, _float3 vLimitAngleMax, CParticle::PARTICLE_TYPE eType);


private:
	void Create_Effect_Sircle(_float3* Out_vDir, _float3 vDir, _float3 vLimitAngleMin, _float3 vLimitAngleMax);




private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


public:
	virtual void Free() override;

};

END

