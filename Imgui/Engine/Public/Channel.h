#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}

public:
	HRESULT Initialize(aiNodeAnim*	pAIChannel);
	_uint Update_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, class CHierarchyNode* pNode);
	_bool Update_LinearTransformation(_float fPlayTime, class CHierarchyNode* pNode, KEYFRAME DestKeyFrame, ANIM_LINEAR_DATA* pData);

	KEYFRAME Get_FirstKeyFrame() { return m_KeyFrames.front(); }
	void	Set_StartLinearKeyFrames(_float fPlayTime, _uint iCurrentKeyFrame);

private:
	char							m_szName[MAX_PATH] = "";

	_uint							m_iNumKeyFrames = 0;
	vector<KEYFRAME>				m_KeyFrames;


	KEYFRAME						m_StartLinearKeyFram;

public:
	static CChannel* Create(aiNodeAnim*	pAIChannel);
	virtual void Free() override;



public: // For. Data
	void Get_ChannelData(DATA_HEROCHANNEL* pData);

};

END