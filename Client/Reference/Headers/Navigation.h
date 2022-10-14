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
	// ���� ���� ���� y���� ���´�
	_float Compute_Height(_fvector  vPos);
	// �̵� �����ϳ�
	_bool isMove(_fvector vPosition);
	// ���� �꿴��
	_bool isGround(_fvector vPosition, _float* OutfCellY);

#ifdef _DEBUG
public:
	HRESULT Render();
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