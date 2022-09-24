#include "..\Public\Animation.h"
#include "Channel.h"
#include "Model.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(aiAnimation * pAIAnimation, CModel* pModel)
{
	m_fDuration = pAIAnimation->mDuration;
	m_fTickPerSecond = pAIAnimation->mTicksPerSecond;

	/* 현재 애니메이션에서 제어해야할 뼈들의 갯수를 저장한다. */
	m_iNumChannels = pAIAnimation->mNumChannels;


	/* 현재 애니메이션에서 제어해야할 뼈정보들을 생성하여 보관한다. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel*		pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Play_Animation(_float fTimeDelta)
{
	m_fPlayTime += m_fTickPerSecond * fTimeDelta;

	if (m_fPlayTime >= m_fDuration)
	{
		m_fPlayTime = 0.f;

		for (auto& pChannel : m_Channels)
		{
			pChannel->Reset_KeyFrameIndex();
		}

	}

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_Transformation(m_fPlayTime);
	}

	return S_OK;
}

CAnimation * CAnimation::Create(aiAnimation * pAIAnimation, CModel* pModel)
{
	CAnimation*			pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
}
