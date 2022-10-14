#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CModel;
class CCell;

END

BEGIN(Client)

class CDataManager final : public CBase
{
#pragma region Data
public:
	typedef struct dataMapObj
	{
		char	cName[MAX_PATH];
		_float3 vPos;
		_float3 vAngle;
		_float3 vScale;

	}DATA_MAP_OBJ;

	typedef struct dataMap
	{
		_int iID;
		_int iNumObj;
		DATA_MAP_OBJ* pObjDatas;

	}DATA_MAP;


	typedef struct dataLenearAnim
	{
		char	cName[MAX_PATH];
		_int	iNumData;
		ANIM_LINEAR_DATA* pLinearDatas;

	}DATA_LENEARANIM;


	typedef struct dataCell
	{
		_float3		vPoints[3];
		_int		iNeighborIndex[3];
	}DATA_CELL;
	typedef struct dataNavi
	{
		_int iID;
		_uint iNumCell;
		DATA_CELL* pCellDatas;

	}DATA_NAVI;

#pragma endregion

	DECLARE_SINGLETON(CDataManager)

public:
	enum DATA_TYPE { DATA_ANIM, DATA_NOEANIM, DATA_PARTS, DATA_END };

public:
	CDataManager();
	virtual ~CDataManager() = default;


	// For. Static
public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	HRESULT ReadSceneData(char* pFileName, DATA_HEROSCENE* pScene, DATA_TYPE eTYPE);

	HRESULT Create_Try_BinModel(const _tchar* pModelName, LEVEL eLEVEL, DATA_TYPE eTYPE);







	// For. Pass
public:
	const list<string>* Get_AnimFileNames() { return &m_AnimFilePaths; }
	const list<string>* Get_NonAnimFileNames() { return &m_NonAnimFilePaths; }
	const list<string>* Get_PartFileNames() { return &m_PartFilePaths; }

private:
	HRESULT  LoadModelPass();

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	list<string>	m_AnimFilePaths;
	list<string>	m_NonAnimFilePaths;
	list<string>	m_PartFilePaths;







	// For. Data
public:
	HRESULT Load_Map(_int iMapID, LEVEL eLEVEL);

	vector<CCell*> Load_Navi(_int iMapID);

	list<ANIM_LINEAR_DATA> Load_Anim(char* pFileName);


public:
	virtual void Free() override;

};

END

