#include "../Default/stdafx.h"
#include "..\Public\ImGui_Manager.h"
#include "Texture.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "CamManager.h"
#include "MapManager.h"
#include "MeshManager.h"
#include "DataManager.h"
#include "AnimManager.h"
#include "PartsManager.h"

#include "ColorCube.h"
#include "MarkCube.h"
#include "LookCube.h"
#include "StaticModel.h"
#include "AnimModel.h"
#include "Player.h"
#include "MultiThread.h"
#include "Collider.h"

#include "Transform.h"

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

	m_temp = io.ConfigFlags;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

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
		Render_CamTool();
		break;
	case Client::LEVEL_CAMTOOL:
		Render_CamTool();
		break;
	case Client::LEVEL_ANIMTOOL:
		Render_AnimTool();
		Render_CamTool();
		break;
	case Client::LEVEL_PARTSTOOL:
		Render_PartsTool();
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
	
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

}





void CImGui_Manager::Init_Level(LEVEL eCurLevel)
{
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
	case Client::LEVEL_ANIMTOOL:
		Init_AnimTool();
		break;
	case Client::LEVEL_PARTSTOOL:
		Init_PartsTool();
		break;
	case Client::LEVEL_TESTLEVEL:
		Init_TestLevel();
		break;
	}

}
void CImGui_Manager::Clear_Level()
{
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
	case Client::LEVEL_ANIMTOOL:
		Clear_AnimTool();
		break;
	case Client::LEVEL_PARTSTOOL:
		Clear_PartsTool();
		break;
	case Client::LEVEL_TESTLEVEL:
		Clear_TestLevel();
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
void CImGui_Manager::Init_AnimTool()
{
}
void CImGui_Manager::Init_PartsTool()
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
	else if (ImGui::Button("AnimTool"))
		CToolManager::Get_Instance()->Change_Level(LEVEL_ANIMTOOL);
	else if (ImGui::Button("PartsTool"))
		CToolManager::Get_Instance()->Change_Level(LEVEL_PARTSTOOL);
	else if (ImGui::Button("TestTool"))
		CToolManager::Get_Instance()->Change_Level(LEVEL_TESTLEVEL);


	ImGui::End();
}
void CImGui_Manager::Render_MapTool()
{
	Render_StaticTool();


	Window_Terrain();
	
	Window_Model();

	if (CMapManager::Get_Instance()->Get_NaviMode())
	{
		Window_Navi();
		if (nullptr != CMeshManager::Get_Instance()->Get_MultiThread())
			Window_LoadingReadyNeighbor();
	}
	else
	{
		Window_CreatedModel();
		Window_Transform();
	}


	if (CMapManager::Get_Instance()->Get_ColMode())
	{
		Window_Col();
	}



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
void CImGui_Manager::Render_AnimTool()
{
	Render_StaticTool();


	// Player Mode
	CGameObject* pObj = nullptr;
	if (*CAnimManager::Get_Instance()->Get_PlayMode())
	{
		CAnimManager::Get_Instance()->Delete_Model();
		pObj = CAnimManager::Get_Instance()->Get_PlayerModel();
		Window_PlayMode();
	}
	// Anim Mode
	else
	{
		CAnimManager::Get_Instance()->Delete_Player();
		pObj = CAnimManager::Get_Instance()->Get_AnimModel();
		Window_AnimModleList();
		if (nullptr != pObj)
			Window_AnimEditor();
	}


	// Transform
	if (nullptr != pObj)
	{
		CTransform* pTransform = (CTransform*)pObj->Get_ComponentPtr(TEXT("Com_Transform"));
		_float3* pAixs = pObj->Get_Axis();
		Window_Transform(pTransform, pAixs);
	}


	if (CAnimManager::Get_Instance()->Get_IsDelete())
		Window_IsDelete();


}
void CImGui_Manager::Render_PartsTool()
{
	Render_StaticTool();

	Window_Part();
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
	{
		CToolManager::Get_Instance()->Change_Level(LEVEL_SELECTTOOL);
		CImGui_Manager::Get_Instance()->Clear_Level();
	}



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

	CMapManager::Get_Instance()->Free();
	CMeshManager::Get_Instance()->Free();

	CCamManager::Get_Instance()->Free();
}
void CImGui_Manager::Clear_CamTool()
{

	CCamManager::Get_Instance()->Free();

}
void CImGui_Manager::Clear_AnimTool()
{

	CAnimManager::Get_Instance()->Free();
	CCamManager::Get_Instance()->Free();
}
void CImGui_Manager::Clear_PartsTool()
{

	CPartsManager::Get_Instance()->Free();

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




void CImGui_Manager::Window_Terrain()
{
	ImGui::Begin("Terrain Box");


	_float fTemp = CMapManager::Get_Instance()->Get_Height();
	if (ImGui::InputFloat("Height", &fTemp))
		CMapManager::Get_Instance()->Set_Height(fTemp);
	fTemp = CMapManager::Get_Instance()->Get_Rad();
	if (ImGui::InputFloat("Rad", &fTemp))
		CMapManager::Get_Instance()->Set_Rad(fTemp);
	fTemp = CMapManager::Get_Instance()->Get_Sharp();
	if (ImGui::InputFloat("Sharp", &fTemp))
		CMapManager::Get_Instance()->Set_Sharp(fTemp);

	ImGui::End();
}

void CImGui_Manager::Window_Model()
{
	ImGui::Begin("Model Box");

	const list<string>* FileNames = CDataManager::Get_Instance()->Get_NonAnimFileNames();
	
	CMapManager::Get_Instance()->Reset_CulLIst();

	ImVec2 vSize{ 150.f, 200.f };
	if (ImGui::BeginListBox("Models", vSize))
	{
		int iCount = 0;
		for (auto& sModelName : *FileNames)
		{
			bool isSelected = CMapManager::Get_Instance()->Get_PickedString() == sModelName;
			vSize= { 100.f, 10.f };
			if (ImGui::Selectable(sModelName.data(), isSelected, 0, vSize))
			{
				CMapManager::Get_Instance()->Set_PickedString(sModelName);
			}


			ImGui::SameLine();
			ImVec4 vColor;
			if (*(CMapManager::Get_Instance()->Get_CulBool(iCount)))
				vColor = { 0.f, 1.f, 0.f, 1.f };
			else
				vColor = { 0.f, 0.f, 0.f, 1.f };

			string sCulName = "Culling_" + sModelName;
			if (ImGui::ColorButton(sCulName.data(), vColor, 0, vSize))
			{
				if (!(*CMapManager::Get_Instance()->Get_CulBool(iCount)))
					*CMapManager::Get_Instance()->Get_CulBool(iCount) = true;
				else
					*CMapManager::Get_Instance()->Get_CulBool(iCount) = false;
			}

			if (*(CMapManager::Get_Instance()->Get_CulBool(iCount)))
				CMapManager::Get_Instance()->Add_CulList(sModelName);
			++iCount;


			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndListBox();
	}

	if(ImGui::Button("Create"))
		CMapManager::Get_Instance()->Make_PickedModel();
	ImGui::SameLine();
	_bool bMouseCul = CMapManager::Get_Instance()->Get_MouseCul();
	if (ImGui::Checkbox("Mouse Culling", &bMouseCul))
		CMapManager::Get_Instance()->Set_MouseCul(bMouseCul);


	_bool bNaviMode = CMapManager::Get_Instance()->Get_NaviMode();
	if (ImGui::Checkbox("NaviMode", &bNaviMode))
		CMapManager::Get_Instance()->Set_NaviMode(bNaviMode);

	_bool bColMode = CMapManager::Get_Instance()->Get_ColMode();
	if (ImGui::Checkbox("ColMode", &bColMode))
		CMapManager::Get_Instance()->Set_ColMode(bColMode);

	ImGui::End();
}

void CImGui_Manager::Window_CreatedModel()
{
	ImGui::Begin("CreatedModel Box");

	ImVec2 vSize{ 150.f, 200.f };
	const map<string, CStaticModel*>* tempModels = CMapManager::Get_Instance()->Get_StaticModels();
	if (ImGui::BeginListBox("CreatedModels", vSize))
	{
		for (auto& sModel : *tempModels)
		{
			bool isSelected = CMapManager::Get_Instance()->Get_PickedCreatedString() == sModel.first;
			ImVec2 vSize{ 100.f, 10.f };
			if (ImGui::Selectable(sModel.first.data(), isSelected, 0, vSize))
				CMapManager::Get_Instance()->Set_PickedCreatedString(sModel.first);


			
			CStaticModel* pStaticModel = CMapManager::Get_Instance()->Get_PickedCreatedModel();
			if (nullptr != pStaticModel)
			{
				CModel* pModel = (CModel*)pStaticModel->Get_ComponentPtr(TEXT("Com_Model"));
				if (pModel->Get_IsBin())
				{
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Bin");
				}
			}
		

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (ImGui::Button("Delete"))
		CMapManager::Get_Instance()->Delete_Model();
	ImGui::SameLine();
	if (ImGui::Button("Delete_ALL"))
		CMapManager::Get_Instance()->Delete_Model_All();
	
	_bool bRendPickingColor = CMapManager::Get_Instance()->Get_RendPickingColor();
	if (ImGui::Checkbox("Rend_PickingColor", &bRendPickingColor))
		CMapManager::Get_Instance()->Set_RendPickingColor(bRendPickingColor);
	_bool bRendCulMode = CMapManager::Get_Instance()->Get_RendCulMode();
	if (ImGui::Checkbox("Rend_CulMode", &bRendCulMode))
		CMapManager::Get_Instance()->Set_RendCulMode(bRendCulMode);


	if (ImGui::Button("Convert To Bin"))
		CMapManager::Get_Instance()->Conv_PickedModel_To_Bin();
	ImGui::SameLine();
	if(ImGui::Button("Convert To Bin All"))
		CMapManager::Get_Instance()->Conv_AllModel_To_Bin();

	if (ImGui::Button("AutoGen"))
		CMapManager::Get_Instance()->AutoGen_ClickedModel();


	ImGui::PushItemWidth(25.f);

	_int ID = CMapManager::Get_Instance()->Get_ID();
	if (ImGui::InputInt("Map_ID", &ID, 0))
		CMapManager::Get_Instance()->Set_ID(ID);

	if (ImGui::Button("Save_Map"))
		CMapManager::Get_Instance()->Save_MapData();
	ImGui::SameLine();
	if (ImGui::Button("Load_Map"))
		CMapManager::Get_Instance()->Load_MapData();
	
	ImGui::PopItemWidth();
		


	ImGui::End();
}

void CImGui_Manager::Window_Transform()
{

	CStaticModel* pPickedModel = CMapManager::Get_Instance()->Get_PickedCreatedModel();

	if (nullptr == pPickedModel)
		return;

	CTransform* pTransform = (CTransform*)pPickedModel->Get_ComponentPtr(TEXT("Com_Transform"));
	_float3* vAixs = pPickedModel->Get_Axis();
	Window_Transform(pTransform, vAixs);


}

void CImGui_Manager::Window_Navi()
{
	ImGui::Begin("Navi Tool");



	ImGui::PushItemWidth(30.f);

	_float fCosRatio = CMeshManager::Get_Instance()->Get_CosRatio();
	if (ImGui::InputFloat("CosRatio", &fCosRatio))
		CMeshManager::Get_Instance()->Set_CosRatio(fCosRatio);

	_float fMaxArea = CMeshManager::Get_Instance()->Get_MaxArea();
	if (ImGui::InputFloat("MaxArea", &fMaxArea))
		CMeshManager::Get_Instance()->Set_MaxArea(fMaxArea);

	if ((ImGui::Button("Bake Navi")))
		CMeshManager::Get_Instance()->Comput_AllObjNaviMesh();
	ImGui::SameLine();
	if (ImGui::Button("Find Neighbor"))
		CMeshManager::Get_Instance()->Ready_Neighbor();

	_float fRendRange = CMeshManager::Get_Instance()->Get_RendRange();
	if (ImGui::InputFloat("Rend Cells Range", &fRendRange))
		CMeshManager::Get_Instance()->Set_RendRange(fRendRange);
	ImGui::PopItemWidth();

	_bool bClickVertexModel = CMeshManager::Get_Instance()->Get_ClickVertexMode();
	if (ImGui::Checkbox("ClickVertexModel", &bClickVertexModel))
		CMeshManager::Get_Instance()->Set_ClickVertexMode(bClickVertexModel);

	_uint iClickedCellIndex = CMeshManager::Get_Instance()->Get_ClickedCell();
	ImGui::Text("ClickedCellIndex: %d", iClickedCellIndex);

	_float3 vClickedCellPos = CMeshManager::Get_Instance()->Get_ClickedCellPos();
	ImGui::Text("CllickedCellos: %.2f/%.2f/%.2f", vClickedCellPos.x, vClickedCellPos.y, vClickedCellPos.z);



	ImGui::PushItemWidth(25.f);

	_int ID = CMeshManager::Get_Instance()->Get_MapID();
	if (ImGui::InputInt("Navi_ID", &ID, 0))
		CMeshManager::Get_Instance()->Set_MapID(ID);

	if (ImGui::Button("Save_Navi"))
		CMeshManager::Get_Instance()->Save_NaviData();
	ImGui::SameLine();
	if (ImGui::Button("Load_Navi"))
		CMeshManager::Get_Instance()->Load_NaviData();

	ImGui::PopItemWidth();



	ImGui::End();
}

void CImGui_Manager::Window_LoadingReadyNeighbor()
{
	ImGui::Begin("ReadyNeighbor Loading...");

	
	CMultiThread* pLoading = CMeshManager::Get_Instance()->Get_MultiThread();

	_float iPersent = (pLoading->Get_iNumReadyNeighbor() / (_float)pLoading->Get_iNumReadyNeighborMax()) * 100.f;

	ImGui::Text("%f / 100.0", iPersent);

	ImGui::End();

}

void CImGui_Manager::Window_Col()
{
	ImGui::Begin("Collider Tool");

	CCollider::COLLIDERDESC ColDesc = CMapManager::Get_Instance()->Get_ClikedColDesc();

	_float3			vCenter = ColDesc.vCenter;
	if (ImGui::DragFloat3("vCenter", (_float*)&vCenter, 0.1f))
	{
		ColDesc.vCenter = vCenter;
		CMapManager::Get_Instance()->Set_ClikedColDesc(ColDesc);
	}

	_float3			vSize = ColDesc.vSize;
	if (ImGui::DragFloat3("vSize", (_float*)&vSize, 0.1f))
	{
		ColDesc.vSize = vSize;
		CMapManager::Get_Instance()->Set_ClikedColDesc(ColDesc);
	}

	_float3			vRotation = ColDesc.vRotation;
	if (ImGui::DragFloat3("vRotation", (_float*)&vRotation, 0.1f))
	{
		ColDesc.vRotation = vRotation;
		CMapManager::Get_Instance()->Set_ClikedColDesc(ColDesc);
	}

	if (ImGui::Button("Apply_ALL_SameName"))
		CMapManager::Get_Instance()->Set_AllSameNameColDesc(ColDesc);

	_bool bWall = CMapManager::Get_Instance()->Get_ClickedWall();
	if (ImGui::Checkbox("isWall", &bWall))
		CMapManager::Get_Instance()->Set_ClickedWall(bWall);


	ImGui::End();
}

void CImGui_Manager::Window_AnimModleList()
{

	ImGui::Begin("Anim Box");



	const list<string>* FileNames = CDataManager::Get_Instance()->Get_AnimFileNames();

	ImVec2 vSize{ 150.f, 200.f };
	if (ImGui::BeginListBox("Models", vSize))
	{
		for (auto& sModelName : *FileNames)
		{
			bool isSelected = CAnimManager::Get_Instance()->Get_PickedString() == sModelName;
			vSize = { 100.f, 10.f };
			if (ImGui::Selectable(sModelName.data(), isSelected, 0, vSize))
				CAnimManager::Get_Instance()->Set_PickedString(sModelName);



			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndListBox();
	}


	if (ImGui::Button("Load by Bin"))
		CAnimManager::Get_Instance()->Create_Model();
	ImGui::SameLine();
	if (ImGui::Button("Load By Scene"))
		CAnimManager::Get_Instance()->Create_ModelByOri();


	if (ImGui::Checkbox("Play Mode", CAnimManager::Get_Instance()->Get_PlayMode()))
	{
		_bool bPlayMode = *CAnimManager::Get_Instance()->Get_PlayMode();
		if (bPlayMode)
			CAnimManager::Get_Instance()->Create_Player();
	}



	ImGui::End();

}

void CImGui_Manager::Window_IsDelete()
{
	ImGui::Begin("******************Warning******************");

	ImGui::Text("There is a model created.");
	ImGui::Text("Are you sure you want to delete it?");

	if (ImGui::Button("Go! Delete!!!"))
	{
		CAnimManager::Get_Instance()->Delete_Model();
		CAnimManager::Get_Instance()->Create_Model();
	}
	ImGui::SameLine();
	if (ImGui::Button("Sorry... It's a mistake"))
	{
		CAnimManager::Get_Instance()->Set_IsDelete(false);
	}


	ImGui::End();

}

void CImGui_Manager::Window_AnimEditor()
{
	ImGui::Begin("AnimEditor");

	if (nullptr == CAnimManager::Get_Instance()->Get_AnimModel())
	{
		ImGui::End();
		return;
	}

	_int iAnimCount = CAnimManager::Get_Instance()->Get_AnimCount(CAnimManager::EDIT_MODEL);
	_int iCurIndex = CAnimManager::Get_Instance()->Get_CurAimIndex(CAnimManager::EDIT_MODEL);
	char* cCurName = CAnimManager::Get_Instance()->Get_CurAnimName(CAnimManager::EDIT_MODEL);

	ImGui::Text("AnimCount: %d", iAnimCount);
	ImGui::Text("CurIndex: %d", iCurIndex);
	ImGui::Text(cCurName);

	if (ImGui::InputInt("Input Index", &iCurIndex))
		CAnimManager::Get_Instance()->Change_Anim(CAnimManager::EDIT_MODEL, iCurIndex);
	if (ImGui::Button("Delete_Anim"))
		CAnimManager::Get_Instance()->Delete_Anim(CAnimManager::EDIT_MODEL, iCurIndex);


	_float fSpeed = CAnimManager::Get_Instance()->Get_AnimSpeed(CAnimManager::EDIT_MODEL);
	if (ImGui::InputFloat("Anim_Speed", &fSpeed))
		CAnimManager::Get_Instance()->Set_AnimSpeed(CAnimManager::EDIT_MODEL, fSpeed);

	_bool bLoop = CAnimManager::Get_Instance()->Get_Loop();
	if (ImGui::Checkbox("Loop", &bLoop))
		CAnimManager::Get_Instance()->Set_Loop(bLoop);


	if (ImGui::Button("Convert_Model"))
		CAnimManager::Get_Instance()->Conv_Bin_Anim();


	ImGui::PushItemWidth(25.f);


	ImGui::Checkbox("KeyTest", CAnimManager::Get_Instance()->Get_KeyTest());
	if (*CAnimManager::Get_Instance()->Get_KeyTest())
	{
		ImGui::InputInt("A", CAnimManager::Get_Instance()->Get_KeyA(), 0); ImGui::SameLine();
		ImGui::InputInt("D", CAnimManager::Get_Instance()->Get_KeyD(), 0); ImGui::SameLine();
		ImGui::InputInt("Idle", CAnimManager::Get_Instance()->Get_KeyNone(), 0);
	}


	ImGui::PopItemWidth();




	ImGui::End();
}

void CImGui_Manager::Window_PlayMode()
{
	ImGui::Begin("Player Tool");
	
	if (nullptr == CAnimManager::Get_Instance()->Get_PlayerModel())
	{
		ImGui::End();
		return;
	}


	_int iAnimCount = CAnimManager::Get_Instance()->Get_AnimCount(CAnimManager::EDIT_PLAYER);
	_int iCurIndex = CAnimManager::Get_Instance()->Get_CurAimIndex(CAnimManager::EDIT_PLAYER);


	ImGui::Text("AnimCount: %d", iAnimCount);
	ImGui::Text("CurIndex: %d", iCurIndex);


	if (ImGui::Checkbox("Play Mode", CAnimManager::Get_Instance()->Get_PlayMode()))
	{
		_bool bPlayMode = *CAnimManager::Get_Instance()->Get_PlayMode();
		if (bPlayMode)
			CAnimManager::Get_Instance()->Create_Player();
	}
	_bool bGameMode = CAnimManager::Get_Instance()->Get_GameMode();
	if (ImGui::Checkbox("GameMode", &bGameMode))
	{
		CAnimManager::Get_Instance()->Set_GameMode(bGameMode);
	}
	
	if (!bGameMode)
	{
		_bool bAnimTestMode = CAnimManager::Get_Instance()->Get_AnimTestMode();
		if (ImGui::Checkbox("AnimTestMode", &bAnimTestMode))
			CAnimManager::Get_Instance()->Set_AnimTestMode(bAnimTestMode);

		if(bAnimTestMode)
			Window_AnimTestMode();
	}


		
	




	ImGui::Text("======AnimSpeed======");
	_float fSpeed = CAnimManager::Get_Instance()->Get_Player_AnimSpeed(CAnimPlayer::STATE_IDLE);
	if (ImGui::InputFloat("IDLE", &fSpeed))
		CAnimManager::Get_Instance()->Set_Player_AnimSpeed(CAnimPlayer::STATE_IDLE, fSpeed);

	fSpeed = CAnimManager::Get_Instance()->Get_Player_AnimSpeed(CAnimPlayer::STATE_WALK);
	if (ImGui::InputFloat("WALK", &fSpeed))
		CAnimManager::Get_Instance()->Set_Player_AnimSpeed(CAnimPlayer::STATE_WALK, fSpeed);

	fSpeed = CAnimManager::Get_Instance()->Get_Player_AnimSpeed(CAnimPlayer::STATE_RUN);
	if (ImGui::InputFloat("RUN", &fSpeed))
		CAnimManager::Get_Instance()->Set_Player_AnimSpeed(CAnimPlayer::STATE_RUN, fSpeed);

	fSpeed = CAnimManager::Get_Instance()->Get_Player_AnimSpeed(CAnimPlayer::STATE_SLEP);
	if (ImGui::InputFloat("SLEP", &fSpeed))
		CAnimManager::Get_Instance()->Set_Player_AnimSpeed(CAnimPlayer::STATE_SLEP, fSpeed);

	fSpeed = CAnimManager::Get_Instance()->Get_Player_AnimSpeed(CAnimPlayer::STATE_JUMPLENDING);
	if (ImGui::InputFloat("JUMPRENDING", &fSpeed))
		CAnimManager::Get_Instance()->Set_Player_AnimSpeed(CAnimPlayer::STATE_JUMPLENDING, fSpeed);

	fSpeed = CAnimManager::Get_Instance()->Get_Player_AnimSpeed(CAnimPlayer::STATE_RUNJUMPLENDING);
	if (ImGui::InputFloat("RUNJUMPRENDING", &fSpeed))
		CAnimManager::Get_Instance()->Set_Player_AnimSpeed(CAnimPlayer::STATE_RUNJUMPLENDING, fSpeed);

	fSpeed = CAnimManager::Get_Instance()->Get_Player_AnimSpeed(CAnimPlayer::STATE_DOUBLEJUMP);
	if (ImGui::InputFloat("DOUBLEJUMP", &fSpeed))
		CAnimManager::Get_Instance()->Set_Player_AnimSpeed(CAnimPlayer::STATE_DOUBLEJUMP, fSpeed);

	fSpeed = CAnimManager::Get_Instance()->Get_Player_AnimSpeed(CAnimPlayer::STATE_SPRINTJUMP);
	if (ImGui::InputFloat("SPRINTJUMP", &fSpeed))
		CAnimManager::Get_Instance()->Set_Player_AnimSpeed(CAnimPlayer::STATE_SPRINTJUMP, fSpeed);

	fSpeed = CAnimManager::Get_Instance()->Get_Player_AnimSpeed(CAnimPlayer::STATE_SLIDE);
	if (ImGui::InputFloat("SLIDE", &fSpeed))
		CAnimManager::Get_Instance()->Set_Player_AnimSpeed(CAnimPlayer::STATE_SLIDE, fSpeed);

	fSpeed = CAnimManager::Get_Instance()->Get_Player_AnimSpeed(CAnimPlayer::STATE_SLIDELENDING);
	if (ImGui::InputFloat("SLIDERENDING", &fSpeed))
		CAnimManager::Get_Instance()->Set_Player_AnimSpeed(CAnimPlayer::STATE_SLIDELENDING, fSpeed);


	ImGui::Text("======AnimLinear======");
	ImGui::InputInt("MyIndex", CAnimManager::Get_Instance()->Get_iMyIndex());
	ImGui::InputInt("TargetIndex", CAnimManager::Get_Instance()->Get_iTargetIndex());
	ImGui::InputFloat("TickPerSeconed", CAnimManager::Get_Instance()->Get_fTickperSceconed());
	ImGui::InputFloat("LimitRatio", CAnimManager::Get_Instance()->Get_fLimitRatio());
	if(ImGui::Button("Add Info!"))
		CAnimManager::Get_Instance()->Set_AnimLinearData();



	if (ImGui::BeginListBox("LinearDataList"))
	{
		ImGui::PushItemWidth(100.f);

		for (auto& Data : *CAnimManager::Get_Instance()->Get_LinearLists())
		{
			//bool isSelected = CCamManager::Get_Instance()->Get_SelectedMarkCubeTag() == pMarkCube->Get_Tag();
			//ImVec2 vSize{ 80.f, 10.f };
			//if (ImGui::Selectable(pMarkCube->Get_Tag().c_str(), isSelected, 0, vSize))
			//	CCamManager::Get_Instance()->Set_SelectedMarkCubeTag(pMarkCube->Get_Tag());


			

			ImGui::Text("==========================");
			ImGui::Text("MyIndex:        %d", Data.iMyIndex);
			ImGui::Text("TargetIndex:    %d", Data.iTargetIndex);
			ImGui::Text("TickPerSeconed: %f", Data.fTickPerSeconed);
			ImGui::Text("LimitRatio:     %f", Data.fLimitRatio);
			ImGui::Text("==========================");

			



			//if (isSelected)
			//	ImGui::SetItemDefaultFocus();
		}

		ImGui::PopItemWidth();

		ImGui::EndListBox();
	}

	ImGui::PushItemWidth(50.f);

	if (ImGui::Button("SaveData"))
		CAnimManager::Get_Instance()->Save_PlayerAnimData();
	ImGui::SameLine();
	if (ImGui::Button("LoadData"))
		CAnimManager::Get_Instance()->Load_PlayerAnimData();




	ImGui::PopItemWidth();

	ImGui::End();

}

void CImGui_Manager::Window_AnimTestMode()
{
	ImGui::Begin("AnimTestMode");

	_int iStartAnimIndex = CAnimManager::Get_Instance()->Get_StartAnimIndex();
	if (ImGui::InputInt("Start Index: ", &iStartAnimIndex))
	{
		CAnimManager::Get_Instance()->Set_StartAnimIndex(iStartAnimIndex);
		CAnimManager::Get_Instance()->Get_PlayerModel()->Set_AnimIndex(iStartAnimIndex);
	}

	_int iEndAnimIndex = CAnimManager::Get_Instance()->Get_EndAnimIndex();
	if (ImGui::InputInt("End Index: ", &iEndAnimIndex))
	{
		CAnimManager::Get_Instance()->Set_EndAnimIndex(iEndAnimIndex);
		CAnimManager::Get_Instance()->Get_PlayerModel()->Set_AnimIndex(iStartAnimIndex);
	}


	ImGui::End();

}

void CImGui_Manager::Window_Part()
{
	ImGui::Begin("Part Editor");

	const list<string>* FileNames = CDataManager::Get_Instance()->Get_PartFileNames();

	ImVec2 vSize{ 150.f, 200.f };
	if (ImGui::BeginListBox("Parts", vSize))
	{
		for (auto& sModelName : *FileNames)
		{
			bool isSelected = CPartsManager::Get_Instance()->Get_PickedString() == sModelName;
			vSize = { 100.f, 10.f };
			if (ImGui::Selectable(sModelName.data(), isSelected, 0, vSize))
				CPartsManager::Get_Instance()->Set_PickedString(sModelName);

			if (CPartsManager::Get_Instance()->Find_Parts(sModelName))
			{
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Created");
			}


			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndListBox();
	}

	
	ImGui::InputText("BoneName", CPartsManager::Get_Instance()->Get_BoneName(), sizeof(char) * MAX_PATH);

	if (ImGui::Button("Load by Bin"))
		CPartsManager::Get_Instance()->Create_Parts();
	//ImGui::SameLine();
	//if (ImGui::Button("Load By Scene"))
	//	CAnimManager::Get_Instance()->Create_ModelByOri();
	if (ImGui::Button("Delete"))
		CPartsManager::Get_Instance()->Delete_Parts();
	if (ImGui::Button("Convert_AllCreatedParts_To_Bin"))
		CPartsManager::Get_Instance()->Conv_AllCratedModel_To_Bin();

	_int iIndex = CPartsManager::Get_Instance()->Get_AnimIndex();
	if (ImGui::InputInt("Anim", &iIndex))
		CPartsManager::Get_Instance()->Set_AnimIndex(iIndex);


	_bool bStatu = CPartsManager::Get_Instance()->Get_StatuMode();
	if (ImGui::Checkbox("Statu_Mode", &bStatu))
		CPartsManager::Get_Instance()->Set_StatuMode(bStatu);


	CGameObject* pObj = (CGameObject*)CPartsManager::Get_Instance()->Find_Parts(CPartsManager::Get_Instance()->Get_PickedString());
	if (nullptr != pObj)
	{
		CTransform* pTransform = (CTransform*)pObj->Get_ComponentPtr(TEXT("Com_Transform"));
		_float3* pAixs = pObj->Get_Axis();
		Window_Transform(pTransform, pAixs);
	}


	ImGui::End();
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

void CImGui_Manager::Window_Transform(CTransform* pTransform, _float3* vAxis)
{
	ImGui::Begin("Transform");

	if (pTransform)
	{

		_float3 vPos; XMStoreFloat3(&vPos, pTransform->Get_State(CTransform::STATE_POSITION));
		_float3 vScale = pTransform->Get_Scale();


		_float fSensPos = 0.1f;
		if (m_eCurLevel == LEVEL_PARTSTOOL)
			fSensPos = 0.01f;
		if (ImGui::DragFloat3("Pos", (_float*)&vPos, fSensPos))
			pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
		if (ImGui::DragFloat3("Scale", (_float*)&vScale, 0.01f, 0.001f))
			pTransform->Set_Scale(XMLoadFloat3(&vScale));
		if (ImGui::DragFloat3("Rotation", (_float*)vAxis, 0.1f))
			pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), vAxis->x, XMVectorSet(0.f, 1.f, 0.f, 0.f), vAxis->y, XMVectorSet(0.f, 0.f, 1.f, 0.f), vAxis->z);

	}
	else
	{
		ImGui::Text("Please clik Model!");
	}


	ImGui::End();
}

void CImGui_Manager::UI_LoadingBox()
{

	ImGui::Begin("Loading...");

	ImVec2 vPos(500.f, 400.f);
	ImGui::SetNextWindowPos(vPos);


	ImGui::Text("Model is being Loaded... Wait for me!");


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
