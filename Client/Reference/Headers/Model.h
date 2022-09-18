#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const char* pModelFilePath, const char* pModelFileName);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Render();

private:
	const aiScene*				m_pAIScene = nullptr;
	Assimp::Importer			m_Importer;

private:
	_uint									m_iNumMeshes = 0;
	vector<class CMeshContainer*>			m_Meshes;
	typedef vector<class CMeshContainer*>	MESHES;

private:
	HRESULT Ready_MeshContainers();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath, const char* pModelFileName);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END