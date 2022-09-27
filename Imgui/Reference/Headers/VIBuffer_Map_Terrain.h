#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Map_Terrain final : public CVIBuffer
{
public:
	typedef struct tagTerrainDesc
	{
		_uint iNumVerticesX;
		_uint iNumVerticesY;
	}TERRAINDESC;

private:
	CVIBuffer_Map_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Map_Terrain(const CVIBuffer_Map_Terrain& rhs);
	virtual ~CVIBuffer_Map_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Create_TerrainMxM(_uint iNumVerticesX, _uint iNumVerticesZ);

	virtual _bool Picking(class CTransform* pTransform, _float3* pOut);
	virtual _float Compute_Height(_float3 vTargetPos);

public:
	void Make_Tick_Up(_float fHeight, _float fRad, _float fSharp, _float3 vPoint, _float fTimeDelta);

private:
	_int Get_Index(_float3 vPos);
	HRESULT Cul_OptiIndex(_fvector vPos, _fvector vDir);

private:
	_uint			m_iNumVerticesX, m_iNumVerticesZ;
	_int			m_iRange = 25;

	_int m_iStartZ = 0;
	_int m_iStartX = 0;
	_int m_iEndZ = 0;
	_int m_iEndX = 0;

public:
	static CVIBuffer_Map_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END