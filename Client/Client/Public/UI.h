#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI : public CGameObject
{
public:
	enum UI_STATE { UI_HOVER, UI_DOWN, UI_PRESS, UI_CLICK, UI_END };

public:
	typedef struct tagUIInfoDesc
	{
		_float					fX, fY, fSizeX, fSizeY;
		CUI*					pParent = nullptr;
		void*					pDesc = nullptr;
	}UIINFODESC;


protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	_vector Get_TotalPos();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	void UI_InputDebug(_float fTimeDelta);
	void UI_RenderDebug();

public:
	UIINFODESC	Get_Info() { return m_TotalInfo; };
	HRESULT Make_ChildUI(_float fX, _float fY, _float fSizeX, _float fSizeY, _tchar* pTag, void* pArg = nullptr);
	void Delete_AllChildUI();

protected:
	CRenderer*				m_pRendererCom = nullptr;		// 고정
	CTransform*				m_pTransformCom = nullptr;		// 고정
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;		// 고정

protected:
	list<CUI*>				m_pChildUIs;

	UIINFODESC				m_UiInfo;
	UIINFODESC				m_TotalInfo;
	_float4x4				m_ViewMatrix, m_ProjMatrix;

	UI_STATE				m_eState = UI_END;


private:
	HRESULT Ready_Components();
	void Handle_Collision();

	virtual void Handle_Hover() {};
	virtual void Handle_Down() {};
	virtual void Handle_Press() {};
	virtual void Handle_Click() {};

public:
	static CUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END