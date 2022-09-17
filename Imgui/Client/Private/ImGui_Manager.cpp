#include "../Default/stdafx.h"
#include "..\Public\ImGui_Manager.h"
#include "Texture.h"

#include "ToolManager.h"
#include "CamManager.h"
#include "ColorCube.h"
#include "MarkCube.h"
#include "LookCube.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
	:m_bShow_Demo_Window(false)
	,m_eCurLevel(LEVEL_END)
{
}


HRESULT CImGui_Manager::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	// Our state
	m_bShow_Demo_Window = false;
	// m_Clear_Color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	return S_OK;

}
void CImGui_Manager::Tick(_float fTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (m_bShow_Demo_Window)
		ImGui::ShowDemoWindow(&m_bShow_Demo_Window);

	if (m_bWarning)
	{
		UI_WarningBox();
		return;
	}


	switch (m_eCurLevel)
	{
	case Client::LEVEL_GAMEPLAY:
		Render_GamePlay();
		break;
	case Client::LEVEL_SELECTTOOL:
		Render_SelectTool();
		break;
	case Client::LEVEL_MAPTOOL:
		Render_MapTool();
		break;
	case Client::LEVEL_CAMTOOL:
		Render_CamTool();
		break;
	case Client::LEVEL_TESTLEVEL:
		Render_TestLevel();
		break;
	}


	ImGui::EndFrame();
}
void CImGui_Manager::Render()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
}



void CImGui_Manager::Change_Level(LEVEL eCurLevel)
{
	if (eCurLevel == m_eCurLevel)
		return;

	switch (m_eCurLevel)
	{
	case Client::LEVEL_GAMEPLAY:
		Clear_GamePlay();
		break;
	case Client::LEVEL_SELECTTOOL:
		Clear_SelectTool();
		break;
	case Client::LEVEL_MAPTOOL:
		Clear_MapTool();
		break;
	case Client::LEVEL_CAMTOOL:
		Clear_CamTool();
		break;
	case Client::LEVEL_TESTLEVEL:
		Clear_TestLevel();
		break;
	}

	m_eCurLevel = eCurLevel;

	switch (m_eCurLevel)
	{
	case Client::LEVEL_GAMEPLAY:
		Init_GamePlay();
		break;
	case Client::LEVEL_SELECTTOOL:
		Init_SelectTool();
		break;
	case Client::LEVEL_MAPTOOL:
		Init_MapTool();
		break;
	case Client::LEVEL_CAMTOOL:
		Init_CamTool();
		break;
	case Client::LEVEL_TESTLEVEL:
		Init_TestLevel();
		break;
	}

}




void CImGui_Manager::Init_GamePlay()
{
}
void CImGui_Manager::Init_SelectTool()
{
}
void CImGui_Manager::Init_MapTool()
{
}
void CImGui_Manager::Init_CamTool()
{
}
void CImGui_Manager::Init_TestLevel()
{

}




void CImGui_Manager::Render_GamePlay()
{

}
void CImGui_Manager::Render_SelectTool()
{
	ImGui::Begin("SelectTool Box");


	if (ImGui::Button("MapTool"))
		CToolManager::Get_Instance()->Change_Level(LEVEL_MAPTOOL);
	else if (ImGui::Button("CamTool"))
		CToolManager::Get_Instance()->Change_Level(LEVEL_CAMTOOL);
	
	else if (ImGui::Button("TestTool"))
		CToolManager::Get_Instance()->Change_Level(LEVEL_TESTLEVEL);


	ImGui::End();
}
void CImGui_Manager::Render_MapTool()
{
	Render_StaticTool();

	ImGui::Begin("MapTool Box");
	




	ImGui::End();
}
void CImGui_Manager::Render_CamTool()
{
	Render_StaticTool();
	ImGui::Begin("CamTool Box");
	


	UI_MarkCubes();
	


	if (CCamManager::Get_Instance()->Get_Start())
	{
		if (ImGui::ColorButton("Stop", ImVec4{ 255.f, 0.f, 0.f, 255.f }))
			CCamManager::Get_Instance()->Set_Start(false);
	}
	else
	{
		if (ImGui::Button("Play"))
			CCamManager::Get_Instance()->Set_Start(true);
	}

	ImGui::SameLine();

	if (ImGui::Button("SetPos"))
		CCamManager::Get_Instance()->Set_SelectingCubePosToSelectedCube();

	ImGui::SameLine();

	_bool bShowCube = CCamManager::Get_Instance()->Get_ShowCube();
	if (ImGui::Checkbox("ShowCube", &bShowCube))
		CCamManager::Get_Instance()->Set_ShowCube(bShowCube);




	ImGui::End();
}
void CImGui_Manager::Render_TestLevel()
{
	Render_StaticTool();

	ImGui::Begin("TestLevel Box");





	ImGui::End();
}

void CImGui_Manager::Render_StaticTool()
{
	ImGui::Begin("StaticTool Box");


	ImGui::Checkbox("Demo Window", &m_bShow_Demo_Window);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	if (ImGui::Button("SelectTool"))
		CToolManager::Get_Instance()->Change_Level(LEVEL_SELECTTOOL);


	ImGui::End();
}




void CImGui_Manager::Clear_GamePlay()
{

}
void CImGui_Manager::Clear_SelectTool()
{
}
void CImGui_Manager::Clear_MapTool()
{

}
void CImGui_Manager::Clear_CamTool()
{

	CCamManager::Get_Instance()->Free();

}
void CImGui_Manager::Clear_TestLevel()
{

}









void CImGui_Manager::UI_MarkCubes()
{
	const list<CMarkCube*>* pMarkCubes = CCamManager::Get_Instance()->Get_MarkCubes();
	const list<CLookCube*>* pLookCubes = CCamManager::Get_Instance()->Get_LookCubes();


	if (ImGui::BeginListBox("MarkCubeList"))
	{

		for (auto& pMarkCube : *pMarkCubes)
		{
			bool isSelected = CCamManager::Get_Instance()->Get_SelectedMarkCubeTag() == pMarkCube->Get_Tag();
			ImVec2 vSize{ 80.f, 10.f };
			if (ImGui::Selectable(pMarkCube->Get_Tag().c_str(), isSelected, 0, vSize))
				CCamManager::Get_Instance()->Set_SelectedMarkCubeTag(pMarkCube->Get_Tag());


			ImGui::SameLine();
			

			string sButtonTag = "Index: " + pMarkCube->Get_Tag();
			
			ImGui::PushItemWidth(100.f);
			_int iTemp = pMarkCube->Get_LinkIndex();


			if (ImGui::InputInt(sButtonTag.c_str(), &iTemp))
			{
				pMarkCube->Set_LinkIndex(iTemp);
				CCamManager::Get_Instance()->MakeRenderPos();
			}




			// ImGui::SameLine();
			string sTimeTag = "Time: " + pMarkCube->Get_Tag();

			_float fTemp = pMarkCube->Get_SpeedTime();
			if (ImGui::InputFloat(sTimeTag.c_str(), &fTemp))
			{
				pMarkCube->Set_SpeedTime(fTemp);

				CCamManager::Get_Instance()->MakeRenderPos();
			}




			ImGui::PopItemWidth();
			

			//if (0 > pMarkCube->Get_LinkIndex() || 3 < pMarkCube->Get_LinkIndex())
			//{
			//	pMarkCube->Set_LinkIndex(0);
			//	Set_WarningBox("Index range must be 0-3 !!");
			//}


			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete_MarkCube"))
		CCamManager::Get_Instance()->Delete_MarkCube();




	if (ImGui::BeginListBox("LookCubeList"))
	{

		for (auto& pLookCube : *pLookCubes)
		{
			bool isSelected = CCamManager::Get_Instance()->Get_SelectedMarkCubeTag() == pLookCube->Get_Tag();
			ImVec2 vSize{ 80.f, 10.f };
			if (ImGui::Selectable(pLookCube->Get_Tag().c_str(), isSelected, 0, vSize))
				CCamManager::Get_Instance()->Set_SelectedMarkCubeTag(pLookCube->Get_Tag());


			ImGui::SameLine();


			string sButtonTag = "Index: " + pLookCube->Get_Tag();

			ImGui::PushItemWidth(100.f);
			_int iTemp = pLookCube->Get_LinkIndex();


			if (ImGui::InputInt(sButtonTag.c_str(), &iTemp))
			{
				pLookCube->Set_LinkIndex(iTemp);
				CCamManager::Get_Instance()->MakeRenderPos();
			}




			// ImGui::SameLine();
			string sTimeTag = "Time: " + pLookCube->Get_Tag();

			_float fTemp = pLookCube->Get_SpeedTime();
			if (ImGui::InputFloat(sTimeTag.c_str(), &fTemp))
			{
				pLookCube->Set_SpeedTime(fTemp);

				CCamManager::Get_Instance()->MakeRenderPos();
			}




			ImGui::PopItemWidth();


			//if (0 > pMarkCube->Get_LinkIndex() || 3 < pMarkCube->Get_LinkIndex())
			//{
			//	pMarkCube->Set_LinkIndex(0);
			//	Set_WarningBox("Index range must be 0-3 !!");
			//}


			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete_LookCube"))
		CCamManager::Get_Instance()->Delete_LookCube();


}

void CImGui_Manager::Set_WarningBox(const char * cContent)
{
	m_cContent = cContent;
	m_bWarning = true;
}
void CImGui_Manager::UI_WarningBox()
{
	ImGui::Begin("!!!!!!!!!!!!!!!! Warning !!!!!!!!!!!!!!!!");


	ImGui::Text(m_cContent);


	if (ImGui::Button("OK"))
		m_bWarning = false;

	ImGui::End();
}





void CImGui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//CleanupDeviceD3D();
	//::DestroyWindow(hwnd);
	//::UnregisterClass(wc.lpszClassName, wc.hInstance);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
