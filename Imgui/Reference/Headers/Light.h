#pragma once

#include "Base.h"
#include "DebugDraw.h"

BEGIN(Engine)
/* 빛 정볼르 보관한다. */

class ENGINE_DLL CLight final : public CBase
{
private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight() = default;

public:
	LIGHTDESC Get_LightDesc() const {
		return m_LightDesc;
	}
	void Set_LightDesc(LIGHTDESC Desc) { m_LightDesc = Desc ; }

	_uint Get_ID() { return m_iID; }
	void Set_ID(_uint iID) { m_iID = iID; }

public:
	HRESULT Initialize(const LIGHTDESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;



private:
	LIGHTDESC				m_LightDesc;
	_uint					m_iID = 0;

public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	virtual void Free() override;
};

END