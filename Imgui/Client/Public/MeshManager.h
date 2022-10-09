#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CCell;
class CShader;

END

BEGIN(Client)

class CMeshManager final : public CBase
{
	DECLARE_SINGLETON(CMeshManager)
public:
	CMeshManager();
	virtual ~CMeshManager() = default;

public:
	_float Get_CosRatio() { return m_fCosRatio; }
	void Set_CosRatio(_float fCosRatio) { m_fCosRatio = fCosRatio; }

public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	HRESULT Add_Cell(_float fDis, _float3* vPoss);
	
	_bool Check_Cell(_float3* vPoss);
	void Sort_Cell(_float3* vPoss);
	void Sort_CellByDot(_float3* vPoss);

	HRESULT Comput_Cell();

	void Comput_AllObjNaviMesh();
	HRESULT Add_Cell(_float3* vPoss);

	void Clear_Cells();



	HRESULT Render();


private:
	vector<CCell*>			m_Cells;
	typedef vector<CCell*>	CELLS;

	map<_float, _float3*> m_TempCells;

	_float m_fCosRatio = 0.f;

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	class CShader*			m_pShader = nullptr;

public:
	virtual void Free() override;

};

END

