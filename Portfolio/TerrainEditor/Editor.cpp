#include "stdafx.h"
#include "Editor.h"
#include "Brush.h"
#include "Utilities/BinaryFile.h"
#include "Utilities/Xml.h"

void Editor::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(45, 0, 0);
	Context::Get()->GetCamera()->Position(131, 419, -180);
	((Freedom*)Context::Get()->GetCamera())->Speed(150, 2);

	shader = new Shader(L"19_Terrain_Splatting.fx");

	sky = new CubeSky(L"Environment/GrassCube1024.dds");
	
	//Terrain
	{
		terrain = new Terrain(shader, L"Terrain/Gray256.png");
		terrain->BaseMap(L"Terrain/Dirt.png");
		terrain->LayerMap(L"Terrain/Forest Floor.jpg", L"Terrain/Gray256.png");

		//terrain->Pass(NONE);
	}


	AddBillboard();

	heightMapTexture = terrain->HeightMap();
	baseMapTexture = terrain->BaseMap();
	layerMapTexture = terrain->LayerMap();

	brush = new Brush(shader, terrain);
	dataMapFile = L"Gray256.dds";
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
	brush->Update();
	billboard->Update();
}

void Editor::Render()
{
	sky->Render();
	brush->Render();
	terrain->Render();
	billboard->Render();
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
		ImGui::Checkbox("WireFrame", &isWireFrameCheck);

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
	if (ImGui::TreeNode("Terrain Splatting"))
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
	terrain->SetLayerMap();
}

void Editor::ImportHeightMap(wstring files)
{
	if (terrain->HeightMap()->GetFile() == files) return;

	terrain->HeightMap(files);
	terrain->SetHeightMap();
	terrain->SetTerrainData();
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
			L"../../_Textures/Terrain",
			func,
			D3D::GetDesc().Handle
		);
	}
}

void Editor::AddBillboard()
{
	billboardShader = new Shader(L"26_Billboard.fx");

	billboard = new Billboard(billboardShader);
	billboard->AddTexture(L"Terrain/Tree.png");
	billboard->AddTexture(L"Terrain/Tree2.png");
	billboard->AddTexture(L"Terrain/Tree3.png");
	billboard->AddTexture(L"Terrain/Tree4.png");

	UINT terrainWidth = terrain->GetWidth() - 3;
	for (UINT i = 0; i < 20; i++)
	{
		Vector2 scale = Math::RandomVec2(5.0f, 10.0f);
		Vector3 position = Math::RandomVec3(0.0f, terrainWidth);
		position.y = (terrain->GetHeight(position) + (scale.y * 0.5f));

		billboard->Add(position, scale);
	}
}

void Editor::ExportMapFile()
{
	ID3D11Texture2D* srcTexture = heightMapTexture->GetTexture();
	D3D11_TEXTURE2D_DESC srcDesc;
	srcTexture->GetDesc(&srcDesc);

	ID3D11Texture2D* readTexture;
	D3D11_TEXTURE2D_DESC readDesc;
	ZeroMemory(&readDesc, sizeof(D3D11_TEXTURE2D_DESC));
	readDesc.Width = srcDesc.Width;
	readDesc.Height = srcDesc.Height;
	readDesc.ArraySize = 1;
	readDesc.Format = srcDesc.Format;
	readDesc.MipLevels = 1;
	readDesc.SampleDesc = srcDesc.SampleDesc;
	readDesc.Usage = D3D11_USAGE_STAGING;
	readDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	Check(D3D::GetDevice()->CreateTexture2D(&readDesc, NULL, &readTexture));
	D3D::GetDC()->CopyResource(readTexture, srcTexture);

	UINT* pixels = new UINT[readDesc.Width * readDesc.Height];
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(readTexture, 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(pixels, subResource.pData, sizeof(UINT) * readDesc.Width * readDesc.Height);
	}
	D3D::GetDC()->Unmap(readTexture, 0);

	UINT* heights = new UINT[readDesc.Width * readDesc.Height];

	ID3D11Texture2D* saveTexture;
	D3D11_TEXTURE2D_DESC saveDesc;
	ZeroMemory(&saveDesc, sizeof(D3D11_TEXTURE2D_DESC));
	saveDesc.Width = readDesc.Width;
	saveDesc.Height = readDesc.Height;
	saveDesc.ArraySize = 1;
	saveDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	saveDesc.MipLevels = 1;
	saveDesc.SampleDesc = readDesc.SampleDesc;
	saveDesc.Usage = D3D11_USAGE_STAGING;
	saveDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA saveSubResource;
	saveSubResource.pSysMem = heights;
	saveSubResource.SysMemPitch = sizeof(UINT) * readDesc.Width;
	saveSubResource.SysMemSlicePitch = sizeof(UINT) * readDesc.Width * readDesc.Height;

	Check(D3D::GetDevice()->CreateTexture2D(&readDesc, &saveSubResource, &saveTexture));

	wstring fileName = Path::GetFileNameWithoutExtension(heightMapTexture->GetFile());
	fileName = L"../../_Textures/Terrain/" + fileName + L".dds";

	Check(D3DX11SaveTextureToFile(D3D::GetDC(), saveTexture, D3DX11_IFF_DDS, fileName.c_str()));

	SafeRelease(srcTexture);
	SafeRelease(readTexture);
	SafeRelease(saveTexture);
}

void Editor::UpdateDataMapFile()
{
	dataMapFileList.clear();
	Path::GetFiles(&dataMapFileList, L"../../_Textures/Terrain/", L"*.dds", false);

	for (wstring& file : dataMapFileList)
		file = Path::GetFileNameWithoutExtension(file);
}

void Editor::SaveMapFile(wstring file) {

	Texture* texture = new Texture(L"Terrain/" + dataMapFile);

	ID3D11Texture2D* srcTexture = texture->GetTexture();

	D3D11_TEXTURE2D_DESC srcDesc;
	srcTexture->GetDesc(&srcDesc);

	UINT* pixels = new UINT[terrain->GetWidth() * terrain->GetHeight()];

	UINT i = 0;

	for (UINT z = terrain->GetHeight(); z > 0; z--) {
		for (UINT x = 0; x < terrain->GetWidth(); x++) {
			UINT index = terrain->GetWidth() * (z - 1) + x;
			UINT r, g, b, a;

			r = (UINT)(terrain->Vertices()[index].Color.x * 255) << 0;
			g = (UINT)(terrain->Vertices()[index].Color.y * 255) << 8;
			b = (UINT)(terrain->Vertices()[index].Color.z * 255) << 16;
			if (terrain->Vertices()[index].Position.y * 10 > 0xFF)
				a = 0xFF << 24;
			else if (terrain->Vertices()[index].Position.y < 0)
				a = 0x00 << 24;
			else
				a = (UINT)(terrain->Vertices()[index].Position.y * 10) << 24;

			pixels[i] = r + g + b + a;
			i++;
		}
	}

	ID3D11Texture2D* saveTexture;
	D3D11_TEXTURE2D_DESC saveDesc;
	ZeroMemory(&saveDesc, sizeof(D3D11_TEXTURE2D_DESC));
	saveDesc.Width = terrain->GetWidth();
	saveDesc.Height = terrain->GetHeight();
	saveDesc.ArraySize = 1;
	saveDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	saveDesc.MipLevels = 1;
	saveDesc.SampleDesc = srcDesc.SampleDesc;
	saveDesc.Usage = D3D11_USAGE_STAGING;
	saveDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA saveSubResource;
	saveSubResource.pSysMem = pixels;
	saveSubResource.SysMemPitch = sizeof(UINT) * terrain->GetWidth();
	saveSubResource.SysMemSlicePitch = sizeof(UINT) * terrain->GetWidth() * terrain->GetHeight();

	Check(D3D::GetDevice()->CreateTexture2D(&saveDesc, &saveSubResource, &saveTexture));

	wstring fileName = Path::GetFileNameWithoutExtension(file);

	wstring filePath = L"../../_Textures/Terrain/" + fileName + L".dds";

	Check(D3DX11SaveTextureToFile(D3D::GetDC(), saveTexture, D3DX11_IFF_DDS, filePath.c_str()));

	BinaryWriter* w = new BinaryWriter();
	w->Open(L"../../_Textures/Terrain/" + fileName);

	SafeDelete(w);

	SafeRelease(saveTexture);
}