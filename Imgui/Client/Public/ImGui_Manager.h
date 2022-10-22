#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CTransform;

END


BEGIN(Client)

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

public:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;


public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(_float fTimeDelta);
	void Render();







public:
	void Init_Level(LEVEL eCurLevel);
	void Clear_Level();

	void Set_WarningBox(const char* cContent);






private:
	void Init_GamePlay();
	void Init_SelectTool();
	void Init_MapTool();
	void Init_CamTool();
	void Init_AnimTool();
	void Init_PartsTool();
	void Init_TestLevel();
	



	void Render_GamePlay();
	void Render_SelectTool();
	void Render_MapTool();
	void Render_CamTool();
	void Render_AnimTool();
	void Render_PartsTool();
	void Render_TestLevel();

	void Render_StaticTool();



	void Clear_GamePlay();
	void Clear_SelectTool();
	void Clear_MapTool();
	void Clear_CamTool();
	void Clear_AnimTool();
	void Clear_PartsTool();
	void Clear_TestLevel();



private:
	// FOR. CamTool
	void UI_MarkCubes();

	// FOR. MapTool
	void Window_Terrain();
	void Window_Model();
	void Window_CreatedModel();
	void Window_Transform();

	void Window_Navi();
	void Window_LoadingReadyNeighbor();

	void Window_Col();

	// FOR. AnimTool
	void Window_AnimModleList();
	void Window_IsDelete();
	void Window_AnimEditor();
	void Window_PlayMode();
	void Window_AnimTestMode();

	// FOR. Part
	void Window_Part();


private:
	// FOR. Static
	void UI_WarningBox();
	void Window_Transform(CTransform* pTransform, _float3* vAxis);

public:
	void UI_LoadingBox();


private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	bool m_bShow_Demo_Window;
	bool m_bWarning = false;
	const char* m_cContent = nullptr;


	LEVEL m_eCurLevel;
	ImGuiConfigFlags m_temp;


public:
	virtual void Free() override;


};

END

