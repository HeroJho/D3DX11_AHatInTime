#pragma once
#include "Client_Defines.h"
#include "Base.h"


BEGIN(Client)

class CParticleManager final : public CBase
{
	DECLARE_SINGLETON(CParticleManager)

public:
	enum TYPE { STATE_SIRCLE, STATE_HARFSIRCLE, STATE_DISK, STATE_PERP , STATE_END };

	typedef struct ParticleToolDesc
	{
		_uint iID;
		CParticle::PARTICLEDESC Desc;

		TYPE eType;
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

	}PARTICLETOOLDESC;



public:
	CParticleManager();
	virtual ~CParticleManager() = default;

	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void Tick(_float fTimeDelta);


public: // For. Tool
	_uint Get_ClickedID() { return m_iClickedID; }
	void Set_ClickedID(_uint iID) { m_iClickedID = iID; }

	void Add_Particle();
	void Remove_Particle();

private:
	list<PARTICLETOOLDESC> m_EffectInfos;

	_uint m_iClickedID = 0;

	_uint m_iIDCount;
	_float3 m_vLocalPos;
	_float3 m_vDir;



	// For. Client
public:
	/*�� ��, �θ� ����, �θ� ��, ���� ����, ���� ��, ������, ������ ���� ��, ȸ��, x ����, x ���� ��, x �ӵ�, �߷� ����, �߷� ���� ��, ���� �Ŀ�, ������ Ÿ�� */
	void Create_Particle(const TCHAR* cModelTag, _float3 vParentPos ,_float3 vPos, _float3 vLook, _float3 vScale, _float3 vScaleSpeed, _float3 vRotaion, _float3 vDir, _float fStopValue, _float fSpeed, _bool bGravity, _float fGravityValue, _float fJumpPower, _float fLifeTime);

	/*�� ��, �θ� ����, �θ� ��, ���� ����, ���� ��, ������, ������ ���� ��, ȸ��, x ����, x ���� ��, x �ӵ�, �߷� ����, �߷� ���� ��, ���� �Ŀ�, ������ Ÿ�� */
	void Create_Effect(const TCHAR* cModelTag, _float3 vParentPos ,_float3 vPos, _float3 vLook, _float3 vScale, _float3 vScaleSpeed, _float3 vRotaion, _float3 vDir, _float fStopValue, _float fSpeed, _bool bGravity, _float fGravityValue, _float fJumpPower, _float fLifeTime,
	TYPE eType, _uint iCount, _float fRandPos, _float fRandScale, _float fRandScaleSpeed, _float vRandRotation, _float fRandStopValue, _float fRandSpeed, _float fRandGravityValue, _float fRandJumpPower, _float fRandLifeTime);


private:
	void Create_Effect_Sircle(_float3* Out_vDir, _float3 vDir);
	void Create_Effect_HarfSircle(_float3* Out_vDir, _float3 vDir);
	void Create_Effect_Disk(_float3* Out_vDir);
	void Create_Effect_Perp(_float3* Out_vDir);




private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


public:
	virtual void Free() override;

};

END

