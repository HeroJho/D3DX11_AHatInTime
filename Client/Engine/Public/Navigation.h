#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int			iCurrentIndex = -1;
	}NAVIGATIONDESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(vector<class CCell*>* Cells);
	virtual HRESULT Initialize(void* pArg);


public:
	void Ready_CellCollision(class CGameObject* pGameObject, _int* iIndexs, _int iSize);

public:
	const vector<class CGameObject*>* Get_CurCellColliders();
	_bool Get_GroundCell(_float3* pPoss, class CTransform* pTran);
	_uint Get_CurCellIndex() { return m_NavigationDesc.iCurrentIndex; }

	void Set_NaviIndex(_uint iIndex) { m_NavigationDesc.iCurrentIndex = iIndex; }

public:
	// 나의 셀에 대해 y값을 얻어온다
	_float Compute_Height(_fvector  vPos);
	// 이동 가능하냐
	_bool isMove(_fvector vPosition);
	// 셀에 닿였냐
	_bool isGround(_fvector vPosition, _float* OutfCellY , _float fMagicNum = 0.f);

	_int Find_NaviIndex(_fvector vPos);


#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

private:
	vector<class CCell*>			m_Cells;
	typedef vector<class CCell*>	CELLS;

	NAVIGATIONDESC					m_NavigationDesc;

#ifdef _DEBUG
	class CShader*			m_pShader = nullptr;
#endif // _DEBUG



private:
	HRESULT Ready_Neighbor();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, vector<class CCell*>* Cells);
	CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END