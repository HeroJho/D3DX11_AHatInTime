#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_ColorCube final : public CVIBuffer
{
private:
	CVIBuffer_ColorCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_ColorCube(const CVIBuffer_ColorCube& rhs);
	virtual ~CVIBuffer_ColorCube() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;


public:
	void SetColor(_int r, _int g, _int b);

public:
	static CVIBuffer_ColorCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END