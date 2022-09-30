#pragma once

#include "Base.h"

/* ��������(�������� ��Ӱ���)�� ǥ���ϱ����� aiNode�̴�. */

BEGIN(Engine)

class CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}
	const char* Get_ParentName() const {
		if (nullptr == m_pParent)
			return "";
		return m_pParent->Get_Name();
	}

	_uint Get_Depth() const {
		return m_iDepth;
	}

	_matrix Get_OffSetMatrix() {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

	XMFLOAT4X4 Get_OffSetMatrixXM() {
		return m_OffsetMatrix;
	}

	_matrix Get_CombinedTransformation() {
		return XMLoadFloat4x4(&m_CombinedTransformation);
	}

	_float4x4 Get_OriTransformation() { return m_OriTransformation; }

public:
	void Set_Transformation(_fmatrix Transformation) {
		XMStoreFloat4x4(&m_Transformation, Transformation);
	}

	CHierarchyNode* Get_Parent() { return m_pParent; }

public:
	HRESULT Initialize(aiNode* pAINode, class CHierarchyNode* pParent, _uint iDepth);
	HRESULT Bin_Initialize(DATA_HERONODE* pNode);

	void Set_CombinedTransformation();
	void Set_OffsetMatrix(_fmatrix OffsetMatrix);
	void Set_FindParent(CHierarchyNode* pNode);

private:
	char				m_szName[MAX_PATH] = "";
	_float4x4			m_OffsetMatrix;
	_float4x4			m_OriTransformation;
	_float4x4			m_Transformation;
	_float4x4			m_CombinedTransformation;
	char				m_szParentName[MAX_PATH] = "";
	CHierarchyNode*		m_pParent = nullptr;
	_uint				m_iDepth = 0;

public:
	static CHierarchyNode* Bin_Create(DATA_HERONODE* pNode);
	static CHierarchyNode* Create(aiNode* pAINode, class CHierarchyNode* pParent, _uint iDepth);
	virtual void Free();
};

END