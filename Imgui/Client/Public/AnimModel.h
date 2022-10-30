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
	virtual _float3* Get_Axis() override { return &m_vAxis; };
	TCHAR*		Get_ModelName() { return m_cModelTag; };

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	TCHAR					m_cModelTag[MAX_PATH];
	string					m_sModelNum;
	_float3					m_vAxis;
	LEVEL					m_eModelLevel = LEVEL_END;



	// For. LeanAnim
public:

	void Set_AnimLinearData(ANIM_LINEAR_DATA LinearData);
	void Reset_AnimLinearData();


	void		Set_AnimIndex(_uint iIndex);

	void Set_StartAnimIndex(_uint iStartAnimIndex) { m_iStartAnimIndex = iStartAnimIndex; }
	_uint Get_StartAnimIndex() { return m_iStartAnimIndex; }
	void Set_EndAnimIndex(_uint iEndAnimIndex) { m_iEndAnimIndex = iEndAnimIndex; }
	_uint Get_EndAnimIndex() { return m_iEndAnimIndex; }


private:
	_uint				m_iStartAnimIndex = 0;
	_uint				m_iEndAnimIndex = 0;


private:
	HRESULT Ready_Components();

public:
	static CAnimModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END