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

	baseMapTexture = terrain->BaseMap();
	if (ImGui::ImageButton(baseMapTexture->SRV(), size))
	{
		Path::OpenFileDialog(
			L"",
			L"Every File(*.*)\0*.*\0Text File\0*.txt;*.doc\0",
			L"../../_Textures/",
			bind(&Editor::ImportBaseMap, this, placeholders::_1),
			D3D::GetDesc().Handle
		);
	}

	heightMapTexture = terrain->HeightMap();
	if (ImGui::ImageButton(heightMapTexture->SRV(), size))
	{
		Path::OpenFileDialog(
			L"",
			L"Every File(*.*)\0*.*\0Text File\0*.txt;*.doc\0",
			L"../../_Textures/",
			bind(&Editor::ImportHeightMap, this, placeholders::_1),
			D3D::GetDesc().Handle
		);
	}
	sky->Update();
	terrain->Update();
}

void Editor::Render()
{
	sky->Render();
	terrain->Render();
}

void Editor::ImportBaseMap(wstring files)
{
	if (terrain->BaseMap()->GetFile() == files) return;

	terrain->BaseMap(files);
	terrain->SetBaseMap();
}

void Editor::ImportHeightMap(wstring files)
{
	if (terrain->HeightMap()->GetFile() == files) return;

	terrain->HeightMap(files);
	terrain->SetHeightMap();
}