#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Collider.h"

BEGIN(Engine)

class CGameObject;

END


BEGIN(Client)

class CMapManager final : public CBase
{
	DECLARE_SINGLETON(CMapManager)
public:
	CMapManager();
	virtual ~CMapManager() = default;


public:
	void Tick(_float TimeDelta);

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

	_bool Get_RendPickingColor() { return m_bRendPickingColor; }
	void Set_RendPickingColor(_bool bRendPickingColor) { m_bRendPickingColor = bRendPickingColor; }

	_bool Get_RendCulMode() { return m_bRendCulMode; }
	void Set_RendCulMode(_bool bRendCulMode) { m_bRendCulMode = bRendCulMode; }

	_int Get_ID() { return m_iID; }
	void Set_ID(_int iID) { m_iID = iID; }

	map<string, class CStaticModel*>* Get_CreatedModels() { return &m_StaticModels; }

	_bool Get_NaviMode() { return m_bNaviMode; }
	void Set_NaviMode(_bool bNaviMode) { m_bNaviMode = bNaviMode; }

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

	void Click_Model();
	void UnClick_Model();
	void Move_ClickedModel();
	void Add_TempClickedModel(_float fMinDis, CGameObject* pObj);
	void Cul_MinClickedModel();

private:
	bool GenTag(string* pOut);

private:
	map<string, class CStaticModel*> m_StaticModels;
	string		m_sPickedCreatedString;

	string		m_sPickedString;

	_int		m_iID = 0;


	_bool		m_bNaviMode = false;

	_bool		m_bRendPickingColor = false;
	_bool		m_bRendCulMode = false;

	vector<CGameObject*> m_TempClickedObjects;
	vector<_float>		m_TempClickedDis;
	string		m_sMinObject;
	_float3		m_fMinPos;



	// For. Col
public:
	CCollider::COLLIDERDESC Get_ColDesc() { return m_ColDesc; }
	void Set_ColDesc(CCollider::COLLIDERDESC Desc); 

	_bool Get_ColMode() { return m_bColMode; }
	void Set_ColMode(_bool bColMode) { m_bColMode = bColMode; }

	void Find_ClickedColInfo();

private:
	CCollider::COLLIDERDESC m_ColDesc;
	_bool m_bColMode = false;
	_uint m_iColIndex = 0;




public:
	virtual void Free() override;

};

END

