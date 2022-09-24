#pragma once
#include "Client_Defines.h"
#include "Base.h"

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
	void Change_Level(LEVEL eCurLevel);
	void Set_WarningBox(const char* cContent);






private:
	void Init_GamePlay();
	void Init_SelectTool();
	void Init_MapTool();
	void Init_CamTool();
	void Init_TestLevel();



	void Render_GamePlay();
	void Render_SelectTool();
	void Render_MapTool();
	void Render_CamTool();
	void Render_TestLevel();

	void Render_StaticTool();



	void Clear_GamePlay();
	void Clear_SelectTool();
	void Clear_MapTool();
	void Clear_CamTool();
	void Clear_TestLevel();



private:
	// FOR. CamTool
	void UI_MarkCubes();

	// FOR. MapTool
	void Window_Terrain();
	void Window_Model();
	void Window_CreatedModel();
	void Window_Transform();

private:
	// FOR. Static
	void UI_WarningBox();

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

