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

	ImGui::InputInt("Type", (int*)&brushDesc.Type);
	brushDesc.Type %= 3;

	if (Mouse::Get()->Down(0))
	{
		location = brushDesc.Location;
		brushDesc.Location = location;
	}
	else
	{
		brushDesc.Location = terrain->GetPickedPosition();
	}

	ImGui::InputFloat("Flat Height", &flatHeight);
	ImGui::InputFloat3("Location", brushDesc.Location);
	if (Mouse::Get()->Press(0))
	{
		if (Keyboard::Get()->Press(VK_LSHIFT))
		{
			FlatHeight(brushDesc.Type, flatHeight);
		}

		if (Keyboard::Get()->Press('Z'))
		{
			SmoothHeight(brushDesc.Type);
		}

		if (Keyboard::Get()->Press(VK_LCONTROL))
		{
			RaiseHeight(brushDesc.Type, 0.5f, false);
		}
		else if (Keyboard::Get()->Press('X'))
		{
			RaiseHeight(brushDesc.Type, 0.5f, true);
		}

		if (Keyboard::Get()->Press('C'))
		{
			Paint(brushDesc.Type, 1);
		}
	}

	ImGui::InputInt("Range", (int*)&brushDesc.Range);
	brushDesc.Range %= 20;


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
