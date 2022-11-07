#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_Shop_Slot final : public CUI
{
public:
	enum STATE { STATE_IDLE, STATE_SELETED, STATE_BUY, STATE_END };

public:
	typedef struct tagSlotDesc
	{
		TCHAR sItemTag[MAX_PATH];
	}SLOTDESC;

private:
	CUI_Shop_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Shop_Slot(const CUI_Shop_Slot& rhs);
	virtual ~CUI_Shop_Slot() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Select_Slot();
	void UnSelect_Slot();

	void Start_BuyTick();
	void Buy_Item();

	void Set_StopRend(_bool bStopRend) { m_bStopRend = bStopRend; }

	_bool Get_IsBuyed() { return m_bIsBuyed; }

private:
	void Selected_Tick(_float fTimeDelta);
	void Idle_Tick(_float fTimeDelta);
	void Buy_Tick(_float fTimeDelta);


private:
	HRESULT Ready_Components();

	virtual void Handle_Hover() override;
	virtual void Handle_Down() override;
	virtual void Handle_Press() override;
	virtual void Handle_Click() override;


private:
	STATE m_eState = STATE_END;

	TCHAR m_cItemTag[MAX_PATH];

	_float m_fOriSizeX = 0.f;
	_float m_fOriSizeY = 0.f;
	_float m_fHoverSizeX = 0.f;
	_float m_fHoverSizeY = 0.f;
	_float m_fHoverSpeed = 0.f;

	_float m_fOriMovePosX = 0.f;
	_float m_fOriMovePosY = 0.f;
	_float m_fHoverMovePosY = 0.f;
	_float m_fMoveSpeed = 0.f;


	_bool m_bStopRend = false;
	_bool m_bIsBuyed = false;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;


public:
	static CUI_Shop_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END