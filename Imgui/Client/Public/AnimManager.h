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

	void Change_Anim(_int iIndex);
	void Delete_Anim(_int iIndex);

	void Save_Anim();

public:



private:




	// For. Static
private:
	class CAnimModel* m_pAnimModel = nullptr;
	string		m_sPickedString;

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


public:
	virtual void Free() override;

};

END

