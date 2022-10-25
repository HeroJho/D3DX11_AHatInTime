#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CPartsManager final : public CBase
{
	DECLARE_SINGLETON(CPartsManager)
public:
	CPartsManager();
	virtual ~CPartsManager() = default;

public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);




public:
	string Get_PickedString() { return m_sPickedString; }
	void Set_PickedString(string sPickedString) { m_sPickedString = sPickedString; }

	map<string, class CParts*>* Get_Parts() { return &m_Parts; }
	class CParts* Find_Parts(string sTag);

	void Create_Parts();
	void Delete_Parts();

	void Create_Player();
	void Delete_Player();

	void Conv_AllCratedModel_To_Bin();

	_bool Get_StatuMode() { return m_bStatu; };
	void Set_StatuMode(_bool bStatu);

	char* Get_BoneName() { return m_BoneName; }

	_uint Get_AnimIndex();
	void Set_AnimIndex(_uint iIndex);


private:
	map<string, class CParts*>		m_Parts;
	class CPlayer*					m_pPlayer = nullptr;

	string				m_sPickedString;
	char				m_BoneName[MAX_PATH];

	_bool m_bStatu = false;



private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

public:
	virtual void Free() override;

};

END

