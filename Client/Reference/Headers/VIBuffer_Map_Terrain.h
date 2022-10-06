#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Map_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Map_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Map_Terrain(const CVIBuffer_Map_Terrain& rhs);
	virtual ~CVIBuffer_Map_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesY);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Create_TerrainMxM(_uint iNumVerticesX, _uint iNumVerticesZ);

	virtual _bool Picking(class CTransform* pTransform, _float3* pOut);


private:
	_uint			m_iNumVerticesX, m_iNumVerticesZ;


public:
	static CVIBuffer_Map_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumVerticesX, _uint iNumVerticesY);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END