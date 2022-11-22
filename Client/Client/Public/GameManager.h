#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CGameManager final : public CBase
{
	DECLARE_SINGLETON(CGameManager)
public:
	typedef struct tagWispDesc
	{

		_bool		bIsDeleteSubCon;
		_float		fWispRatio;
		_float4		vWispPos;

	}WISPDESC;


private:
	CGameManager();
	virtual ~CGameManager() = default;

	// For. Static
public:
	void Tick(_float fTimeDelta);
	void Clear_Data();




	// Wisp
public:
	void Set_Wisp(_bool bIsDeleteSubCon, _float fWispRatio, _float3 vWispPos, _uint iCurIndex);

	_uint Get_WispInfoNum() { return m_WispInfos.size(); }
	_bool* Get_DeleteSubCons();
	_float* Get_WispRatios();
	_float4* Get_WispPoss();

	_uint	Get_CurIndex() { return m_iCurIndex; }

	_bool Check_IsInWisp(_fvector vPos);
	_bool Check_IsInWispX(_fvector vPos);
	_bool Check_IsInWispWithOutSubCon(_fvector vPos);



private:
	list<WISPDESC>	m_WispInfos;
	_bool			m_IsDeleteSubCons[256];
	_float			m_WispRatios[256];
	_float4			m_WispPoss[256];

	_uint			m_iCurIndex = 0;




	// Diamond
public:
	_uint Get_Diamond() { return m_iDiamond; }

	void Inc_Diamond(_uint iNum);
	void Dec_Diamond(_uint iNum);

private:
	_uint m_iDiamond = 0;






	// Stage_1
public:
	_bool Check_Stage_1();

	void Dis_MonsterVaultCount();
	_bool Check_MonsterVaultCount();


	_bool Get_JumpVault2() { return m_bJumpMapvalut2; }

	void Set_JumpVault() { m_bJumpMapvalut = true; }
	void Set_JumpVault2() { m_bJumpMapvalut2 = true; }

	void Set_Musroom(_bool bMushroom) { m_bInMushroom = bMushroom; }
	_bool Get_Musroom() { return m_bInMushroom; }

private:
	_uint m_iMonsterVaultCount = 1; // 7
	_bool m_bModVault = false;
	_bool m_bJumpMapvalut = false;
	_bool m_bJumpMapvalut2 = false;

	_bool m_bInMushroom = false;



	// For. SavePoint
public:
	void Set_SavePoint(_uint iNaviIndex, _float3 vNaviPos);
	void Get_NaviPoint(_uint* iNaviIndex, _float3* vNaviPos);

private:
	_uint m_iNaviIndex = 0;
	_float3 m_vNaviPos;


public:
	virtual void Free() override;

};

END