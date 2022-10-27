#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CItem : public CGameObject
{
public:
	typedef struct tagItemDesc
	{
		TCHAR szModelName[MAX_PATH];
		_float3 vPos;
		_float3 vAngle;
		_float3 vScale;

		_uint iCount;

	}ITEMDESC;

	typedef struct tagIvenDesc
	{
		TCHAR szModelName[MAX_PATH];
		_uint iCount;
	}ITEMINVENDESC;


protected:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	void Update_ParentPos();
	virtual HRESULT SetUp_State(_fmatrix StateMatrix) override;

public:
	_bool Get_IsHolded() { return m_bIsHolded; }
	void Set_IsHolded(_bool bIsHolded) { m_bIsHolded = bIsHolded; }


public:
	virtual void Use_Item() = 0; 


protected:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pParentTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

public:
	_float3* Get_Axis() { return &m_vAxis; }
	TCHAR* Get_ModelTag() { return m_InvenDesc.szModelName; }

protected:
	ITEMINVENDESC			m_InvenDesc;
	_float3					m_vAxis;

	_bool					m_bIsHolded = false;

protected:
	HRESULT Ready_Components();

public:
	virtual void Free() override;
};

END