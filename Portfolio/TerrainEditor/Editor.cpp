#include "stdafx.h"
#include "Editor.h"

void Editor::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(0, 0, 0);
	Context::Get()->GetCamera()->Position(0, 10, -30);
	((Freedom*)Context::Get()->GetCamera())->Speed(100, 2);

	shader = new Shader(L"47_TerrainLod.fx");

	sky = new CubeSky(L"Environment/GrassCube1024.dds");
	
	//Terrain
	{
		TerrainLod::InitializeDesc desc =
		{
			shader,
			L"Terrain/Gray512.png",
			1.0f, 16, 5
		};

		terrain = new TerrainLod(desc);
		terrain->BaseMap(L"Terrain/Dirt.png");
		terrain->LayerMap(L"Terrain/Forest Floor.jpg", L"Terrain/Gray512.png");
		terrain->NormalMap(L"Terrain/Dirt_Normal.png");
		terrain->Pass(0);
		//terrain->Pass(1);
	}

	heightMapTexture = terrain->HeightMap();
	baseMapTexture = terrain->BaseMap();
	layerMapTexture = terrain->LayerMap();
	normalMapTexture = terrain->NormalMap();
}

void Editor::Destroy()
{
	SafeDelete(terrain);
}

void Editor::Update()
{
	ImGui::Begin("Inspector", &showTerrainEditor);
	{
		AddSettingTypeCombobox();
		
		ImGui::End();
	}
	
	sky->Update();
	terrain->Update();
}

void Editor::Render()
{
	sky->Render();
	terrain->Render();
}

Texture * Editor::GetMapTexture(MapTypes mapTypes)
{
	switch (mapTypes)
	{
	case MapTypes::BASE_MAP:
		return terrain->BaseMap();
		break;
	case MapTypes::LAYER_MAP:
		return terrain->LayerMap();
		break;
	case MapTypes::NORMAL_MAP:
		return terrain->NormalMap();
		break;
	case MapTypes::HEIGHT_MAP:
		return terrain->HeightMap();
		break;
	default:
		return NULL;
	}
}

void Editor::AddSettingTypeCombobox()
{
	const char* toolTypes[] = { "None", "Terrain HeightMap", "Terrain Detail", "Terrain Brush" };
	static int item_current_idx = 0;
	const char* combo_label = toolTypes[item_current_idx];
	ImGui::Text("Setting Type");
	if (ImGui::BeginCombo("ToolType", combo_label))
	{
		for (int n = 0; n < IM_ARRAYSIZE(toolTypes); n++)
		{
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(toolTypes[n], is_selected))
				item_current_idx = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (item_current_idx == 0)
	{
		NoneToolType();
	}

	if (item_current_idx == 1)
	{
		HeightMapToolType();
	}

	if (item_current_idx == 2)
	{
		DetailTerrainToolType();
	}

	if (item_current_idx == 3)
	{
		TerrainBrushToolType();
	}
}

void Editor::NoneToolType()
{
	if (ImGui::CollapsingHeader("Terrain Diffuse"))
	{
		if (ImGui::TreeNode("BaseMap"))
		{
			ImGui::Text("Terrain BaseMap");

			GetTextureMap(baseMapTexture, MapTypes::BASE_MAP);
			GetImportTextureMapFunction(func, &Editor::ImportBaseMap);
			AddMapButton(baseMapTexture, btnSize, func);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("LayerMap"))
		{
			ImGui::Text("Terrain LayerMap");

			GetTextureMap(layerMapTexture, MapTypes::LAYER_MAP);
			GetImportTextureMapFunction(func, &Editor::ImportLayerMap);
			AddMapButton(layerMapTexture, btnSize, func);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("NormalMap"))
		{
			ImGui::Text("Terrain NormalMap");

			GetTextureMap(normalMapTexture, MapTypes::NORMAL_MAP);
			GetImportTextureMapFunction(func, &Editor::ImportNormalMap);
			AddMapButton(normalMapTexture, btnSize, func);

			ImGui::TreePop();
		}
	}
}

void Editor::HeightMapToolType()
{
	if (ImGui::TreeNode("HeightMap"))
	{
		ImGui::Text("Terrain HeightMap");

		GetTextureMap(heightMapTexture, MapTypes::HEIGHT_MAP);
		GetImportTextureMapFunction(func, &Editor::ImportHeightMap);
		AddMapButton(heightMapTexture, btnSize, func);

		ImGui::TreePop();
	}
}

void Editor::DetailTerrainToolType()
{
	if (ImGui::TreeNode("Terrain Detail"))
	{
		ImGui::Text("Terrain FillMode");

		static bool isWireFrameCheck = false;
		ImGui::Checkbox("checkbox", &isWireFrameCheck);

		if (isWireFrameCheck)
		{
			ChangeFillMode(WIRE_FRAME);
		}
		else
		{
			ChangeFillMode(NONE);
		}

		ImGui::TreePop();
	}
}

void Editor::TerrainBrushToolType()
{
	if (ImGui::TreeNode("Terrain Detail"))
	{
		ImGui::Text("");
		ImGui::TreePop();
	}
}

void Editor::ImportBaseMap(wstring files)
{
	if (terrain->BaseMap()->GetFile() == files) return;

	terrain->BaseMap(files);
	terrain->SetBaseMap();
}

void Editor::ImportLayerMap(wstring files)
{
	if (terrain->LayerMap()->GetFile() == files) return;

	terrain->LayerMap(files);
	terrain->SetBaseMap();
}

void Editor::ImportNormalMap(wstring files)
{
	if (terrain->NormalMap()->GetFile() == files) return;

	terrain->NormalMap(files);
	terrain->SetNormalMap();
}

void Editor::ImportHeightMap(wstring files)
{
	if (terrain->HeightMap()->GetFile() == files) return;

	terrain->HeightMap(files);
	terrain->SetHeightMap();
}

function<void(wstring)> Editor::GetImportTextureMapFunction(function<void(wstring)>& func, void(Editor::* function)(wstring files))
{
	return func = bind(function, this, placeholders::_1);
	
}

Texture * Editor::GetTextureMap(Texture *& mapTexture, MapTypes mapTypes)
{
	return mapTexture = GetMapTexture(mapTypes);
}


void Editor::AddMapButton(Texture * mapTexture, ImVec2 size, function<void(wstring)> func)
{
	if (ImGui::ImageButton(mapTexture->SRV(), size))
	{
		Path::OpenFileDialog(
			L"",
			L"Every File(*.*)\0*.*\0Text File\0*.txt;*.doc\0",
			L"../../_Textures/",
			func,
			D3D::GetDesc().Handle
		);
	}
}
