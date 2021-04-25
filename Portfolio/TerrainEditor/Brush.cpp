#include "Framework.h"
#include "Brush.h"

Brush::Brush(Shader* shader, Terrain* terrain)
	: shader(shader), terrain(terrain)
{
	brushBuffer = new ConstantBuffer(&brushDesc, sizeof(BrushDesc));
	sBrushBuffer = shader->AsConstantBuffer("CB_Brush");

	GetTerrainList();
}

Brush::~Brush()
{
	SafeDelete(brushBuffer);
	SafeDelete(terrain);
}

void Brush::Update()
{
	ImGui::Begin("Brush");
	
	//ImGui::InputInt("Type", (int*)&brushDesc.Type);
	//brushDesc.Type %= 3;
	ImGui::RadioButton("None", (int*)&brushDesc.Type, 0); ImGui::SameLine();
	ImGui::RadioButton("Square", (int*)&brushDesc.Type, 1); ImGui::SameLine();
	ImGui::RadioButton("Circle", (int*)&brushDesc.Type, 2);

	static bool bStay = false;
	ImGui::Checkbox("Fixed Position", &bStay);

	ImGui::SliderInt("Range", (int*)&brushDesc.Range, 0, 20);
	brushDesc.Range %= 20;
	if (bStay && Mouse::Get()->Press(0))
	{
		location = brushDesc.Location;
		brushDesc.Location = location;
	}
	else
	{
		brushDesc.Location = terrain->GetPickedPosition();
		location = brushDesc.Location;
	}

	ImGui::InputFloat("Flat Height", &flatHeight);
	ImGui::InputFloat3("Location", brushDesc.Location);

	ImGui::RadioButton("Raise Up", &brushOption, 0);
	ImGui::RadioButton("Raise Down", &brushOption, 1);
	ImGui::RadioButton("Flat", &brushOption, 2);
	ImGui::RadioButton("Smooth", &brushOption, 3);

	if (Mouse::Get()->Press(0))
	{
		switch (brushOption)
		{
		case 0:
			RaiseHeight(brushDesc.Type, 0.5f, true);
			break;
		case 1:
			RaiseHeight(brushDesc.Type, 0.5f, false);
			break;
		case 2:
			FlatHeight(brushDesc.Type, flatHeight);
			break;
		case 3:
			SmoothHeight(brushDesc.Type);
			break;
		default:
			break;
		}
	}

	ImGui::End();
}

void Brush::Render()
{
	brushBuffer->Apply();
	sBrushBuffer->SetConstantBuffer(brushBuffer->Buffer());
}

void Brush::RaiseHeight(UINT type, float intensity, bool bRaise)
{
	UINT left = (UINT)location.x - brushDesc.Range;
	UINT right = (UINT)location.x + brushDesc.Range;
	UINT bottom = (UINT)location.z - brushDesc.Range;
	UINT top = (UINT)location.z + brushDesc.Range;

	if (left < 0) left = 0;
	if (right >= terrain->GetWidth()) right = terrain->GetWidth();
	if (bottom < 0) bottom = 0;
	if (top >= terrain->GetHeight()) top = terrain->GetHeight();

	for (UINT z = bottom; z <= top; z++)
	{
		for (UINT x = left; x <= right; x++)
		{
			UINT index = terrain->GetWidth() * z + x;

			if (index > terrain->GetWidth() * terrain->GetHeight()) return;

			if (type == 1)
			{
				if (bRaise)
					terrain->Vertices()[index].Position.y += intensity * Time::Delta();
				else
					terrain->Vertices()[index].Position.y -= intensity * Time::Delta();
			}

			else if (type == 2)
			{
				Vector2 temp = Vector2(terrain->Vertices()[index].Position.x,
					terrain->Vertices()[index].Position.z);

				float distance = D3DXVec2Length(&(Vector2(location.x, location.z) - temp));

				if (distance < brushDesc.Range)
				{
					if (bRaise)
						terrain->Vertices()[index].Position.y += cos(Math::PI * intensity * distance / brushDesc.Range) * 5.0f * Time::Delta();
					else
						terrain->Vertices()[index].Position.y -= cos(Math::PI * intensity * distance / brushDesc.Range) * 5.0f * Time::Delta();
				}
			}
		}
	}
	terrain->CreateNormalData();
	terrain->UpdateVertexData();
}

void Brush::FlatHeight(UINT type, float targetHeight)
{
	UINT left = (UINT)location.x - brushDesc.Range;
	UINT right = (UINT)location.x + brushDesc.Range;
	UINT bottom = (UINT)location.z - brushDesc.Range;
	UINT top = (UINT)location.z + brushDesc.Range;

	if (left < 0) left = 0;
	if (right >= terrain->GetWidth()) right = terrain->GetWidth();
	if (bottom < 0) bottom = 0;
	if (top >= terrain->GetHeight()) top = terrain->GetHeight();


	for (UINT z = bottom; z <= top; z++) 
	{
		for (UINT x = left; x <= right; x++) 
		{
			UINT index = terrain->GetWidth() * z + x;

			if (index < terrain->GetWidth() * terrain->GetHeight())
			{
				if (type == 1) 
				{
					terrain->Vertices()[index].Position.y = targetHeight;
				}
				else if (type == 2) 
				{
					Vector2 temp = Vector2(terrain->Vertices()[index].Position.x, terrain->Vertices()[index].Position.z);

					float distance = D3DXVec2Length(&(Vector2(location.x, location.z) - temp));

					if (distance < brushDesc.Range)
						terrain->Vertices()[index].Position.y = targetHeight;
				}
			}
		}
	}
	terrain->CreateNormalData();
	terrain->UpdateVertexData();
}

void Brush::SmoothHeight(UINT type)
{
	UINT left = (UINT)location.x - brushDesc.Range;
	UINT right = (UINT)location.x + brushDesc.Range;
	UINT bottom = (UINT)location.z - brushDesc.Range;
	UINT top = (UINT)location.z + brushDesc.Range;

	if (left < 0) left = 0;
	if (right >= terrain->GetWidth()) right = terrain->GetWidth();
	if (bottom < 0) bottom = 0;
	if (top >= terrain->GetHeight()) top = terrain->GetHeight();

	float* GetHeights = new float[terrain->GetWidth() * terrain->GetHeight()];

	Vector2 temp[9] =
	{
		Vector2(-1, +1), Vector2(+0, +1), Vector2(+1, +1),
		Vector2(-1, +0), Vector2(+0, +0), Vector2(+1, +0),
		Vector2(-1, -1), Vector2(+0, -1), Vector2(+1, -1)
	};

	for (UINT z = bottom; z <= top; z++) 
	{
		for (UINT x = left; x <= right; x++) 
		{
			UINT index = terrain->GetWidth() * z + x;

			float avg = 0.0f;

			UINT count = 0;

			if (index < terrain->GetWidth() * terrain->GetHeight()) 
			{
				if (type == 1) 
				{
					for (UINT i = 0; i < 9; i++) 
					{
						int index = terrain->GetWidth() * (z + temp[i].y) + (x + temp[i].x);

						if (index < 0) continue;

						avg += terrain->Vertices()[index].Position.y;
						count++;
					}
					avg /= count;

					GetHeights[terrain->GetWidth() * z + x] = avg;
				}
				else if (type == 2) 
				{
					Vector2 tempVector = Vector2(terrain->Vertices()[terrain->GetWidth() * z + x].Position.x, terrain->Vertices()[terrain->GetWidth() * z + x].Position.z);

					float distance = D3DXVec2Length(&(Vector2(location.x, location.z) - tempVector));

					if (distance < brushDesc.Range) 
					{
						for (UINT i = 0; i < 9; i++) 
						{
							int index = terrain->GetWidth() * (z + temp[i].y) + (x + temp[i].x);

							if (index < 0) continue;

							avg += terrain->Vertices()[index].Position.y;
							count++;
						}
						avg /= count;

						GetHeights[terrain->GetWidth() * z + x] = avg;
					}
				}
			}
		}
	}

	for (UINT z = bottom; z <= top; z++) 
	{
		for (UINT x = left; x <= right; x++) 
		{
			UINT index = terrain->GetWidth() * z + x;

			if (index < terrain->GetWidth() * terrain->GetHeight()) 
			{
				if (type == 1) 
				{
					terrain->Vertices()[index].Position.y = GetHeights[index];
				}
				else if (type == 2) 
				{
					Vector2 tempVector = Vector2(terrain->Vertices()[terrain->GetWidth() * z + x].Position.x, terrain->Vertices()[terrain->GetWidth() * z + x].Position.z);

					float distance = D3DXVec2Length(&(Vector2(location.x, location.z) - tempVector));

					if (distance < brushDesc.Range) 
					{
						terrain->Vertices()[index].Position.y = GetHeights[index];
					}
				}
			}
		}
	}

	terrain->CreateNormalData();
	terrain->UpdateVertexData();

	delete[] GetHeights;
}

void Brush::Paint(UINT type, UINT layer)
{
	UINT left = (UINT)location.x - brushDesc.Range;
	UINT right = (UINT)location.x + brushDesc.Range;
	UINT bottom = (UINT)location.z - brushDesc.Range;
	UINT top = (UINT)location.z + brushDesc.Range;

	if (left < 0) left = 0;
	if (right >= terrain->GetWidth()) right = terrain->GetWidth();
	if (bottom < 0) bottom = 0;
	if (top >= terrain->GetHeight()) top = terrain->GetHeight();

	for (UINT z = bottom; z <= top; z++) 
	{
		for (UINT x = left; x <= right; x++) 
		{
			UINT index = terrain->GetWidth() * z + x;

			if (index < terrain->GetWidth() * terrain->GetHeight()) 
			{
				if (type == 1) 
				{
					terrain->pixels[index].g += 3.0 * Time::Delta();
					terrain->pixels[index].r += 3.0 * Time::Delta();
				}
			}
		}
	}
	terrain->CreateNormalData();

	terrain->SetVertexData();
}

void Brush::AddModel(UINT num)
{
	UINT left = (UINT)location.x - brushDesc.Range;
	UINT right = (UINT)location.x + brushDesc.Range;
	UINT bottom = (UINT)location.z - brushDesc.Range;
	UINT top = (UINT)location.z + brushDesc.Range;

	if (left < 0) left = 0;
	if (right >= terrain->GetWidth()) right = terrain->GetWidth();
	if (bottom < 0) bottom = 0;
	if (top >= terrain->GetHeight()) top = terrain->GetHeight();

	float* GetHeights = new float[terrain->GetWidth() * terrain->GetHeight()];


}

void Brush::Splatting()
{
}

void Brush::GetTerrainList()
{
	terrainFiles.clear();
	Path::GetFiles(&terrainFiles, L"../../_Textures/Terrain/", L"*.jpg", false);

	for (wstring& file : terrainFiles)
	{
		file = L"Terrain/" + Path::GetFileNameWithoutExtension(file) + L".jpg";
		terrainTextures.push_back(new Texture(file));
	}
}

void Brush::OpenMesh()
{
	Path::OpenFileDialog(
		L"",
		L"mesh File\0*.mesh",
		L"../../_Models/",
		bind(&Brush::LoadMesh, this, placeholders::_1),
		D3D::GetDesc().Handle
	);
}

void Brush::LoadMesh(wstring file)
{
	//String::Replace(&file, L"\\", L"/");

	//size_t index = file.find(L"/_Models/");

	//file = file.substr(index + 9, file.length());

	//String::Replace(&file, L"." + Path::GetExtension(file), L"");


	//ModelRender* modelRender = new ModelRender(shader);
	//modelRender->ReadMesh(file);
	//modelRender->ReadMaterial(file);
	//modelRender->AddTransform();
	//modelRender->Pass(static_cast<UINT>(PASS::PASS_MODEL));

	//ModelClass* modelClass = new ModelClass();
	//modelClass->modelRender = modelRender;
	//modelClass->Path = file;
	//modelClass->ModelName = file.substr(file.find_last_of(L"/") + 1, file.length());
	////modelClass->originBoneCount = modelRender->GetModel()->BoneCount();
	//modelClass->Spheres = CreateSpheres(modelClass->modelRender);

	//models.emplace_back(modelClass);
}
