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

	_float Get_RendRange() { return m_fRendRange; }
	void Set_RendRange(_float fRendRange) { m_fRendRange = fRendRange; }

	_float Get_MaxArea() { return m_fMaxArea; }
	void Set_MaxArea(_float fMaxArea) { m_fMaxArea = fMaxArea; }

	vector<CCell*>*	Get_Cells() { return &m_Cells; }

	class CMultiThread* Get_MultiThread();

	_bool Get_ClickVertexMode() { return m_bClickVertexModel; }
	void Set_ClickVertexMode(_bool bClickVertexMode);

	_uint Get_ClickedCell() { return m_iClickedCellIndex; }
	_float3 Get_ClickedCellPos() { return m_vClickedPos; }

public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(_float fTimeDelta);
	HRESULT Render();


public:
	HRESULT Add_Cell(_float fDis, _float3* vPoss);
	HRESULT Delete_Cell();
	class CCell* Find_PickingCell();

	_bool Check_Cell(_float3* vPoss);
	void Sort_Cell(_float3* vPoss);
	void Sort_CellByDot(_float3* vPoss);
	_bool Check_Area(_float3* vPoss);

	HRESULT Comput_Cell();

	void Comput_AllObjNaviMesh();
	HRESULT Add_Cell(_float3* vPoss, _bool bCheckOverlap = false);

	void Clear_Cells();

	HRESULT Ready_Neighbor();

	void Find_CellIndex();

// ==================FreeVertexMode======================

	void Click_Vertex();
	void Clear_ClickedVertex();

	void Create_ClickedVertexCube();
	void Set_ClickedVertexCube();
	void Reset_ClickedVertexCube();
	void Delete_ClickedVertexCube();

	void Move_FreeVectexCube(_float fDis);
	void Comput_FreeVectexCube();

private:
	vector<CCell*>			m_Cells;
	typedef vector<CCell*>	CELLS;

	_uint		m_iClickedCellIndex = 0;
	_float3		m_vClickedPos;

	map<_float, _float3*> m_TempCells;
	vector<_float> m_TempMoveFreeVertax;

	_float m_fCosRatio = 0.f;
	_float m_fRendRange = 0.f;
	_float m_fMaxArea = 0.f;

	_float m_fDeleteKeyTimeAcc = 0.f;

	vector<_float3> m_fvClickedPoss;
	vector<class CClickedVertexCube*> m_ClickedVertexCube;
	class CClickedVertexCube* m_pClickedFreeVerteixCube = nullptr;
	_bool	m_bClickVertexModel = false;


private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	class CMultiThread* m_pLoading = nullptr;

	class CShader*			m_pShader = nullptr;

public:
	virtual void Free() override;

};

END

