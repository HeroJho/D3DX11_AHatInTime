#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CModel;

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

	HRESULT Conv_Bin_Model(CModel* pModel, char* cModelName, DATA_TYPE eTYPE);
	HRESULT SaveSceneData(DATA_HEROSCENE* pScene, char* cModelName, DATA_TYPE eTYPE);
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
	HRESULT Save_Map(_int iMapID);
	DATA_MAP* Load_Map(_int iMapID);

	HRESULT Save_Anim(char* pFileName, list<ANIM_LINEAR_DATA> Datas);
	list<ANIM_LINEAR_DATA> Load_Anim(char* pFileName);

private:
	list<DATA_MAP*> m_Data_Maps;



public:
	virtual void Free() override;

};

END

