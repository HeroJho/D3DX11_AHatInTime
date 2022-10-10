#include "..\Public\Animation.h"
#include "Channel.h"
#include "Model.h"
#include "HierarchyNode.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_Channels(rhs.m_Channels)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fPlayTime(rhs.m_fPlayTime)
	, m_bLoop(rhs.m_bLoop)
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);

	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CAnimation::Initialize_Prototype(aiAnimation * pAIAnimation)
{
	m_fDuration = pAIAnimation->mDuration;
	m_fTickPerSecond = pAIAnimation->mTicksPerSecond;
	strcpy_s(m_szName, pAIAnimation->mName.data);

	/* 현재 애니메이션에서 제어해야할 뼈들의 갯수를 저장한다. */
	m_iNumChannels = pAIAnimation->mNumChannels;


	/* 현재 애니메이션에서 제어해야할 뼈정보들을 생성하여 보관한다. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel*		pChannel = CChannel::Create(pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(CModel* pModel)
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_ChannelKeyFrames.push_back(0);

		CHierarchyNode*		pNode = pModel->Get_HierarchyNode(m_Channels[i]->Get_Name());

		if (nullptr == pNode)
			return E_FAIL;

		m_HierarchyNodes.push_back(pNode);

		Safe_AddRef(pNode);
	}

	return S_OK;
}

HRESULT CAnimation::Bin_Initialize_Prototype(DATA_HEROANIM* pAIAnimation)
{
	m_fDuration = pAIAnimation->fDuration;
	m_fTickPerSecond = pAIAnimation->fTickPerSecond;
	m_bLoop = pAIAnimation->bLoop;

	strcpy(m_szName, pAIAnimation->szName);

	/* 현재 애니메이션에서 제어해야할 뼈들의 갯수를 저장한다. */
	m_iNumChannels = pAIAnimation->iNumChannels;


	/* 현재 애니메이션에서 제어해야할 뼈정보들을 생성하여 보관한다. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel*		pChannel = CChannel::Bin_Create(&pAIAnimation->pHeroChannel[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Bin_Initialize(CModel* pModel)
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_ChannelKeyFrames.push_back(0);

		CHierarchyNode*		pNode = pModel->Get_HierarchyNode(m_Channels[i]->Get_Name());

		if (nullptr == pNode)
			return E_FAIL;

		m_HierarchyNodes.push_back(pNode);

		Safe_AddRef(pNode);
	}

	return S_OK;
}

_bool CAnimation::Play_Animation(_float fTimeDelta)
{
	m_fPlayTime += m_fTickPerSecond * fTimeDelta;

	if (m_fPlayTime >= m_fDuration)
	{
		if (m_bLoop)
			Init_PlayInfo();

		return true;
	}

	_uint		iChannelIndex = 0;

	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex], m_HierarchyNodes[iChannelIndex]);

		++iChannelIndex;
	}

	return false;
}

_bool CAnimation::Play_Animation(ANIM_LINEAR_DATA * pData, list<KEYFRAME>* pFirstKeyFrames, _float fTimeDelta)
{

	_float fRatio = m_fPlayTime / m_fDuration;


	if (!m_bStartLinear)
	{
		if (pData->fLimitRatio < fRatio)
		{
			_uint		iChannelIndex = 0;
			for (auto& pChannel : m_Channels)
			{
				pChannel->Set_StartLinearKeyFrames(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex]);
				++iChannelIndex;
			}

			m_bStartLinear = true;
			m_fPlayTime = 0;
		}
		else
		{
			m_fPlayTime += m_fTickPerSecond * fTimeDelta;

			_uint		iChannelIndex = 0;

			for (auto& pChannel : m_Channels)
			{
				m_ChannelKeyFrames[iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex], m_HierarchyNodes[iChannelIndex]);

				++iChannelIndex;
			}
		}
	}
	else
	{
		m_fPlayTime += pData->fTickPerSeconed * fTimeDelta;

		_uint		iChannelIndex = 0;

		list<KEYFRAME>::iterator iter = pFirstKeyFrames->begin();

		for (auto& pChannel : m_Channels)
		{
			if (pChannel->Update_LinearTransformation(m_fPlayTime, m_HierarchyNodes[iChannelIndex], *iter, pData))
			{
				// m_bStartLinear = false;
				Init_PlayInfo();
				return true;
			}
			++iChannelIndex;
			++iter;
		}
	}

	return false;
}

void CAnimation::Get_FirstKeys(list<KEYFRAME>* pFirstKeys)
{
	for (auto& pChannel : m_Channels)
	{
		pFirstKeys->push_back(pChannel->Get_FirstKeyFrame());
	}
}

void CAnimation::Init_PlayInfo()
{
	m_fPlayTime = 0.f;
	m_bStartLinear = false;

	for (auto& pChannel : m_Channels)
	{

	}

	for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
		iCurrentKeyFrame = 0;
}

CAnimation * CAnimation::Create(aiAnimation * pAIAnimation)
{
	CAnimation*			pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize_Prototype(pAIAnimation)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone(CModel* pModel)
{
	CAnimation*			pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Initialize(pModel)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Bin_Create(DATA_HEROANIM* pAIAnimation)
{
	CAnimation*			pInstance = new CAnimation();

	if (FAILED(pInstance->Bin_Initialize_Prototype(pAIAnimation)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Bin_Clone(CModel* pModel)
{
	CAnimation*			pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Bin_Initialize(pModel)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();


	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_Release(pHierarchyNode);

	m_HierarchyNodes.clear();
}



void CAnimation::Get_AnimData(DATA_HEROANIM * pData)
{

	pData->fDuration = m_fDuration;
	pData->iNumChannels = m_iNumChannels;
	pData->fTickPerSecond = m_fTickPerSecond;
	pData->bLoop = m_bLoop;
	strcpy_s(pData->szName, m_szName);

	pData->pHeroChannel = new DATA_HEROCHANNEL[m_iNumChannels];

	for (_int i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Get_ChannelData(&pData->pHeroChannel[i]);
	}

}
