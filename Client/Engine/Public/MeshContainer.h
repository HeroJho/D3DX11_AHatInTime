#pragma once
#include "VIBuffer.h"


/* 모델을 구성하는 하나의 메시. */
/* 이 메시를 구성하는 정점, 인덱스 버퍼를 보관한다. */

BEGIN(Engine)

class CMeshContainer final : public CVIBuffer
{
private:
	CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh);
	virtual HRESULT Initialize(void* pArg);	

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END