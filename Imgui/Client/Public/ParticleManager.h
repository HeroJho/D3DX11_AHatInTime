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

	void Tick(_float fTimeDelta);


public: // For. Tool
	_int Get_ClickedID() { return m_iClickedID; }
	void Set_ClickedID(_int iID) { m_iClickedID = iID; }

	list<PARTICLETOOLDESC*>*  Get_ParticleInfos() { return &m_EffectInfos; }

	void Add_Particle(CParticle::PARTICLE_TYPE eType);
	PARTICLETOOLDESC* Find_Particle();
	void Remove_Particle();

	TCHAR* Get_ParticleName() { return m_szParticleName; }
	void Set_ParticleName(TCHAR* szName) { lstrcpy(m_szParticleName, szName); }

private:
	list<PARTICLETOOLDESC*> m_EffectInfos;

	_int m_iClickedID = -1;

	_uint m_iIDCount = 0;
	_float3 m_vLocalPos;
	_float3 m_vDir;



	// For. Client
public:
	/*모델 명, 부모 포스, 부모 룩, 로컬 포스, 로컬 룩, 스케일, 스케일 감쇄 값, 회전, x 방향, x 감쇄 값, x 속도, 중력 여부, 중력 감쇄 값, 점프 파워, 라이프 타임 */
	void Create_Particle(const TCHAR* cModelTag, _float3 vParentPos ,_float3 vPos, _float3 vLook, _float3 vScale, _float3 vScaleSpeed, _float3 vRotaion, _float3 vDir, _float fStopValue, _float fSpeed, _bool bGravity, _float fGravityValue, _float fJumpPower, _float fLifeTime, CParticle::PARTICLE_TYPE eType);

	/*모델 명, 부모 포스, 부모 룩, 로컬 포스, 로컬 룩, 스케일, 스케일 감쇄 값, 회전, x 방향, x 감쇄 값, x 속도, 중력 여부, 중력 감쇄 값, 점프 파워, 라이프 타임 */
	void Create_Effect(const TCHAR* cModelTag, _float3 vParentPos, _float3 vPos, _float3 vLook, _float3 vScale, _float3 vScaleSpeed, _float3 vRotaion, _float3 vDir, _float fStopValue, _float fSpeed, _bool bGravity, _float fGravityValue, _float fJumpPower, _float fLifeTime,
		_uint iCount, _float fRandPos, _float fRandScale, _float fRandScaleSpeed, _float vRandRotation, _float fRandStopValue, _float fRandSpeed, _float fRandGravityValue, _float fRandJumpPower, _float fRandLifeTime, _float3 vLimitAngleMin, _float3 vLimitAngleMax, CParticle::PARTICLE_TYPE eType);


private:
	void Create_Effect_Sircle(_float3* Out_vDir, _float3 vDir, _float3 vLimitAngleMin, _float3 vLimitAngleMax);




private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	_float fTimeAcc = 0.f;

	TCHAR m_szParticleName[MAX_PATH];


public:
	virtual void Free() override;

};

END

