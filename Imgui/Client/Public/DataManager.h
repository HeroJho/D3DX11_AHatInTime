#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CModel;

END

BEGIN(Client)

class CDataManager final : public CBase
{
	DECLARE_SINGLETON(CDataManager)
public:
	CDataManager();
	virtual ~CDataManager() = default;


	// For. Static
public:
	HRESULT Init();

	HRESULT SampleSceneData(CModel* pModel);
	HRESULT SaveSceneData(DATA_HEROSCENE* pScene);


public: // For. Map
	const list<string>* Get_NonAnimFileNames() { return &m_NonAnimFilePaths; }
	const list<string>* Get_AnimFileNames() { return &m_AnimFilePaths; }

private:
	HRESULT  LoadModelPass();

private:
	list<string>	m_NonAnimFilePaths;
	list<string>	m_AnimFilePaths;




public:
	virtual void Free() override;

};

END

