#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(aiNodeAnim*	pAIChannel, class CModel* pModel);
	void Update_Transformation(_float fPlayTime);

	void Reset_KeyFrameIndex() {
		m_iCurrentKeyFrame = 0;
	}

private:
	char							m_szName[MAX_PATH] = "";

	_uint							m_iNumKeyFrames = 0;
	vector<KEYFRAME>				m_KeyFrames;

	class CHierarchyNode*			m_pHierarchyNode = nullptr;

	/* 현재 재생되고 있던 키프레임. */
	_uint							m_iCurrentKeyFrame = 0;

public:
	static CChannel* Create(aiNodeAnim*	pAIChannel, CModel* pModel);
	virtual void Free() override;
};

END