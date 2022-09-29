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

class CAnimModel final : public CGameObject
{
public:
	typedef struct tagAnimModelDesc
	{
		TCHAR		cModelTag[MAX_PATH];
	}ANIMMODELDESC;

private:
	CAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimModel(const CAnimModel& rhs);
	virtual ~CAnimModel() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	_int Get_AnimCount();
	_int Get_CurAnimIndex();
	void Set_AnimIndex(_int iIndex);
	void Delete_Anim(_int iIndex);


	virtual _float3* Get_Axis() override { return &m_vAxis; };


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	TCHAR					m_cModelTag[MAX_PATH];
	string					m_sModelNum;
	_float3					m_vAxis;


private:
	HRESULT Ready_Components();

public:
	static CAnimModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END