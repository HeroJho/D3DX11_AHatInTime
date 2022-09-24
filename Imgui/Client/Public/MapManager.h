#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CMapManager final : public CBase
{
	DECLARE_SINGLETON(CMapManager)
public:
	CMapManager();
	virtual ~CMapManager() = default;

	// For. Terrain
public:
	_float	Get_Height() { return m_fHeight; }
	void	Set_Height(_float fHeight) { m_fHeight = fHeight; }
	_float	Get_Rad() { return m_fRad; }
	void	Set_Rad(_float fRad) { m_fRad = fRad; }
	_float  Get_Sharp() { return m_fSharp; }
	void	Set_Sharp(_float fSharp) { m_fSharp = fSharp; }

private:
	_float m_fHeight = 0.f;		// ������ ����
	_float m_fRad = 0.f;		// ������ ������
	_float m_fSharp = 0.f;





	// For. Model
public:
	string Get_PickedString() { return m_sPickedString; }
	void Set_PickedString(string sPickedString) { m_sPickedString = sPickedString; }

	const map<string, class CStaticModel*>* Get_StaticModels() { return &m_StaticModels; }

	string Get_PickedCreatedString() { return m_sPickedCreatedString; }
	void Set_PickedCreatedString(string sPickedCreatedString) { m_sPickedCreatedString = sPickedCreatedString; }

public:
	void Make_Model();
	void Add_Model(class CStaticModel* pStaticModel);
	
	class CStaticModel* Get_PickedCreatedModel();
	class CStaticModel* Find_Model(string sTag);


private:
	bool GenTag(string* pOut);

private:
	map<string, class CStaticModel*> m_StaticModels;
	string		m_sPickedCreatedString;
	
	string		m_sPickedString;
	


public:
	virtual void Free() override;

};

END

