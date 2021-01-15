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

	heightMapTexture = NULL;
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

			baseMapTexture = terrain->BaseMap();
			//TODO: Modify func to put ImportMapTypes for make it work by assign MapTypes variable.
			func = bind(&Editor::ImportBaseMap, this, placeholders::_1);
			AddMapButton(baseMapTexture, size, func);
			
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("LayerMap"))
		{
			ImGui::Text("Terrain LayerMap");

			layerMapTexture = terrain->LayerMap();
			func = bind(&Editor::ImportLayerMap, this,  placeholders::_1);
			AddMapButton(layerMapTexture, size, func);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("NormalMap"))
		{
			ImGui::Text("Terrain NormalMap");

			normalMapTexture = terrain->NormalMap();
			func = bind(&Editor::ImportNormalMap, this, placeholders::_1);
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

void Editor::ImportMapTypes(wstring files, MapTypes mapTypes)
{
	switch (mapTypes)
	{
	case MapTypes::BASE_MAP:
		ImportBaseMap(files);
		break;
	case MapTypes::LAYER_MAP:
		ImportLayerMap(files);
		break;
	case MapTypes::NORMAL_MAP:
		ImportNormalMap(files);
		break;
	case MapTypes::HEIGHT_MAP:
		ImportHeightMap(files);
		break;
	default:
		break;
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
