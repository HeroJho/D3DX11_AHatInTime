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


	HRESULT Play_Animation(_float fTimeDelta);
	_bool Play_Animation(ANIM_LINEAR_DATA* pData, list<KEYFRAME>* pFirstKeyFrames, _float fTimeDelta);

public:
	void	Get_FirstKeys(list<KEYFRAME>* pFirstKeys);
	_float Get_TickPerSecond() { return m_fTickPerSecond; }
	void Set_TickPerSecond(_float fTickPerSecond) { m_fTickPerSecond = fTickPerSecond; }

	void Init_PlayInfo();

	char* Get_Name() { return m_szName; }

private:
	/* �� �ִϸ��̼��� �����ϱ����� ���Ǵ� ���� ����. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;

	/* �ִϸ��̼� ����ϴµ� �ɸ��� ��ü�ð�. */
	_float						m_fDuration = 0.f;

	/* �ִϸ��̼��� �ʴ� ��� �ӵ�. */
	_float						m_fTickPerSecond = 0.f;

	_float						m_fPlayTime = 0.f;

	_bool						m_bStartLinear = false;

	char						m_szName[MAX_PATH];

private: /* ������ �ִϸ��̼� ���� ���� ������. */
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