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
	_float m_fHeight = 0.f;		// 원뿔의 높이
	_float m_fRad = 0.f;		// 원뿔의 반지름
	_float m_fSharp = 0.f;





	// For. Model
public:
	string Get_PickedString() { return m_sPickedString; }
	void Set_PickedString(string sPickedString) { m_sPickedString = sPickedString; }

	const map<string, class CStaticModel*>* Get_StaticModels() { return &m_StaticModels; }

	string Get_PickedCreatedString() { return m_sPickedCreatedString; }
	void Set_PickedCreatedString(string sPickedCreatedString) { m_sPickedCreatedString = sPickedCreatedString; }

	_int Get_ID() { return m_iID; }
	void Set_ID(_int iID) { m_iID = iID; }

public:
	void Make_PickedModel();
	void Delete_Model();
	void Delete_Model_All();
	void Add_Model(class CStaticModel* pStaticModel);

	class CStaticModel* Get_PickedCreatedModel();

	class CStaticModel* Find_Model(string sTag);

	void Conv_PickedModel_To_Bin();

	void Save_MapData();
	void Load_MapData();

private:
	bool GenTag(string* pOut);

private:
	map<string, class CStaticModel*> m_StaticModels;
	string		m_sPickedCreatedString;

	string		m_sPickedString;

	_int		m_iID = 0;





public:
	virtual void Free() override;

};

END

