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
	ImVec2 size = ImVec2(100.0f, 100.0f);                     // Size of the image we want to make visible
	ImGui::Begin("Inspector", &showTerrainEditor);
	{
		const char* toolTypes[] = { "None" };
		ImGui::Text("Setting Type");
		if (ImGui::BeginCombo("ToolType", "None"))
		{
			if (ImGui::Selectable("Terrain HeightMap", false))
			{
				//TODO: Make HeightMap combo.
				ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (ImGui::TreeNode("BaseMap"))
		{
			ImGui::Text("Terrain BaseMap");

			UpdateMapImage(baseMapTexture, MapTypes::BASE_MAP, func, &Editor::ImportBaseMap);
			AddMapButton(baseMapTexture, size, func);
			
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("LayerMap"))
		{
			ImGui::Text("Terrain LayerMap");

			UpdateMapImage(layerMapTexture, MapTypes::LAYER_MAP, func, &Editor::ImportLayerMap);
			AddMapButton(layerMapTexture, size, func);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("NormalMap"))
		{
			ImGui::Text("Terrain NormalMap");

			UpdateMapImage(normalMapTexture, MapTypes::NORMAL_MAP, func, &Editor::ImportNormalMap);
			AddMapButton(normalMapTexture, size, func);

			ImGui::TreePop();
		}
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

void Editor::UpdateMapImage(Texture *& mapTexture, MapTypes mapTypes, function<void(wstring)>& func, void(Editor::*function)(wstring files))
{
	mapTexture = GetMapTexture(mapTypes);
	func = bind(function, this, placeholders::_1);
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
