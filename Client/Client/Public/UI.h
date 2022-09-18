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
	typedef struct tagUIInfoDesc
	{
		_float					fX, fY, fSizeX, fSizeY;
	}UIINFODESC;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	UIINFODESC	Get_Info() { return m_UiInfo; };
	


protected:
	CRenderer*				m_pRendererCom = nullptr;		// 고정
	CTransform*				m_pTransformCom = nullptr;		// 고정
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;		// 고정

protected:
	list<CUI*>				m_pChildUIs;

	UIINFODESC				m_UiInfo;
	_float4x4				m_ViewMatrix, m_ProjMatrix;

protected:
	HRESULT Ready_Components();

public:
	static CUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END