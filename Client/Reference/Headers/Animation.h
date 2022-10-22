#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize_Prototype(aiAnimation* pAIAnimation);
	HRESULT Initialize(class CModel* pModel);
	HRESULT Bin_Initialize_Prototype(DATA_HEROANIM* pAIAnimation);
	HRESULT Bin_Initialize(class CModel* pModel);


	_bool Play_Animation(_float fTimeDelta);
	_bool Play_Animation(ANIM_LINEAR_DATA* pData, list<KEYFRAME>* pFirstKeyFrames, _float fTimeDelta);

public:
	void	Get_FirstKeys(list<KEYFRAME>* pFirstKeys);
	_float Get_TickPerSecond() { return m_fTickPerSecond; }
	void Set_TickPerSecond(_float fTickPerSecond) { m_fTickPerSecond = fTickPerSecond; }

	void Init_PlayInfo();

	char* Get_Name() { return m_szName; }

	_bool Get_Loop() { return m_bLoop; }
	void  Set_Loop(_bool bLoop) { m_bLoop = bLoop; }

private:
	/* 이 애니메이션을 구동하기위해 사용되는 뼈의 갯수. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;
	_bool						m_bLoop = false;

	/* 애니메이션 재생하는데 걸리는 전체시간. */
	_float						m_fDuration = 0.f;

	/* 애니메이션의 초당 재생 속도. */
	_float						m_fTickPerSecond = 0.f;

	_float						m_fPlayTime = 0.f;

	char						m_szName[MAX_PATH];
	_bool						m_bStartLinear = false;


private: /* 복제된 애니메이션 마다 따로 가진다. */
	vector<class CHierarchyNode*>	m_HierarchyNodes;
	vector<_uint>					m_ChannelKeyFrames;

public:
	static CAnimation* Create(aiAnimation* pAIAnimation);
	CAnimation* Clone(class CModel* pModel);

	static CAnimation* Bin_Create(DATA_HEROANIM* pAIAnimation);
	CAnimation* Bin_Clone(class CModel* pModel);


	virtual void Free() override;



public: // For. Data
	void Get_AnimData(DATA_HEROANIM* pData);


};

END