#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CAnimManager final : public CBase
{
	DECLARE_SINGLETON(CAnimManager)
public:
	CAnimManager();
	virtual ~CAnimManager() = default;

public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(_float fTimeDelta);

	// For. Load Model
public:
	string Get_PickedString() { return m_sPickedString; }
	void Set_PickedString(string sPickedString) { m_sPickedString = sPickedString; }

	class CAnimModel* Get_AnimModel() { return m_pAnimModel; }
	class CPlayer*	Get_PlayerModel() { return m_pPlayer; }

	_bool Get_IsLoading() { return m_bIsLoading; }

	_bool Get_IsDelete() { return m_bIsDelete; }
	void  Set_IsDelete(_bool bIsDelete) { m_bIsDelete = bIsDelete; }

public:
	void Make_Model();
	void Create_Model();
	void Load_Model();
	void Delete_Model();


private:
	_bool		m_bIsLoading = false;
	_tchar*     m_cLoadingChar = nullptr;

	_bool		m_bIsDelete = false;







	// For. Edit Anim
public:
	_int Get_AnimCount();
	_int Get_CurAimIndex();

	_int Get_PlayerAnimCount();
	_int Get_CurPlayerAnimIndex();

	void Change_Anim(_int iIndex);
	void Delete_Anim(_int iIndex);

	void Change_PlayerAnim(_int iIndex);

	void Save_Anim();



	int* Get_KeyA() { return &m_iA; }
	int* Get_KeyD() { return &m_iD; }
	int* Get_KeyNone() { return &m_iNone; }
	_bool* Get_KeyTest() { return &m_bKeyTest; }

public:



private:
	int	m_iA = 0;
	int	m_iD = 0;
	int	m_iNone = 0;
	_bool m_bKeyTest = false;

	// For. Static
public:
	void Create_Player();
	_bool* Get_PlayMode() { return &m_bPlayMode; }

private:
	class CAnimModel*	m_pAnimModel = nullptr;
	class CPlayer*		m_pPlayer = nullptr;
	string		m_sPickedString;

	_bool		m_bPlayMode = false;

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


public:
	virtual void Free() override;

};

END

