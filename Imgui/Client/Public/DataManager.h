#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CDataManager final : public CBase
{
	DECLARE_SINGLETON(CDataManager)
public:
	CDataManager();
	virtual ~CDataManager() = default;



public:
	HRESULT Init();




public: // For. Map
	const list<string>* Get_FileNames() { return &m_FilePaths; }

private:
	HRESULT  LoadModelPass();

private:
	list<string>	m_FilePaths;





public:
	virtual void Free() override;

};

END

