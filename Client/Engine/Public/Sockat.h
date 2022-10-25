#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CSockat final : public CComponent
{
public:
	typedef struct tagSocatDesc
	{
		_float4x4		mPivot;
	}SOCATDESC;
	typedef struct tagPartsDsc
	{
		TCHAR				m_szModelName[MAX_PATH];
		_float3				vPos;
		_float3				vRot;
		_float3				vScale;
		class CGameObject*	pOwner = nullptr;
	}PARTSDESC;

private:
	CSockat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSockat(const CSockat& rhs);
	virtual ~CSockat() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	void Tick(_float fTimeDelta, class CTransform* pParentTran);
	void LateTick(_float fTimeDelta, class CRenderer* pRenderer);

public:
	HRESULT Match_Enum_BoneName(char* pBoneName, _int iIndex);
	HRESULT Add_Sockat(_int eID, class CModel* pModel, _tchar* cName, PARTSDESC PartsDesc);
	HRESULT Remove_Sockat(_int eID);
	HRESULT Remove_Sockat_If(_int eID, string sName);
	_bool Check_Sockat(_int eID);

	_bool Check_IsHaveSocket(_int eID, string sName);

private:
	void Update_ParentPos(CTransform* pParentTran);
	HRESULT Add_Child(class CGameObject* pObj, class CHierarchyNode* pHierarchyNode, _int eID);

private:
	vector<class CGameObject*>		m_Parts;
	vector<string>					m_PartsName;

	vector<class CHierarchyNode*>	m_Sockets;
	vector<string>					m_BoneNames;


	_float4x4		m_mPivot;

public:
	static CSockat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END