#include "Framework.h"
#include "Terrain.h"

Terrain::Terrain(Shader * shader, wstring heightFile)
	: Renderer(shader)
{
	heightMap = new Texture(heightFile);

	sBaseMap = shader->AsSRV("BaseMap");
	sLayerMap = shader->AsSRV("LayerMap");
	sAlphaMap = shader->AsSRV("AlphaMap");
	sHeightMap = shader->AsSRV("HeightMap");
	sHeightMap->SetResource(heightMap->SRV());

	sSplattingLayerMap[0] = shader->AsSRV("SplattingLayerMap1");
	sSplattingLayerMap[1] = shader->AsSRV("SplattingLayerMap2");
	
	//buffer = new ConstantBuffer(&bufferDesc, sizeof(BufferDesc));
	//sBuffer = shader->AsConstantBuffer("CB_Terrain");

	lineBuffer = new ConstantBuffer(&lineDesc, sizeof(LineDesc));
	sLineBuffer = shader->AsConstantBuffer("CB_TerrainLine");

	CreateVertexData();
	CreateIndexData();
	CreateNormalData();

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(TerrainVertex), 0, true);
	indexBuffer = new IndexBuffer(indices, indexCount);

	pixels = new Color[width * height];

	vector<Color> colors;
	heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UNORM, &colors);

	for (int i = 0; i < colors.size(); i++)
		pixels[i] = colors[i];

	splattingLayerMap[0] = new Texture(L"Terrain/Dirt.png");
	splattingLayerMap[1] = new Texture(L"Terrain/Rock (Basic).jpg");
}

Terrain::Terrain(Shader* shader, wstring file, bool bDDS)
{
}

Terrain::~Terrain()
{
	SafeDelete(lineBuffer);

	SafeDelete(heightMap);
	SafeDelete(baseMap);
	SafeDelete(layerMap);
	SafeDelete(alphaMap);
	SafeDelete(splattingLayerMap[0]);
	SafeDelete(splattingLayerMap[1]);

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);
	SafeDeleteArray(pixels);
}

void Terrain::Update()
{
	Super::Update();
}

void Terrain::Render()
{
	Super::Render();

	if (baseMap != NULL)
		sBaseMap->SetResource(baseMap->SRV());

	if (layerMap != NULL)
	{
		sLayerMap->SetResource(layerMap->SRV());
		sAlphaMap->SetResource(alphaMap->SRV());
	}

	if (splattingLayerMap[0] != NULL)
		sSplattingLayerMap[0]->SetResource(splattingLayerMap[0]->SRV());

	if (splattingLayerMap[1] != NULL)
		sSplattingLayerMap[1]->SetResource(splattingLayerMap[1]->SRV());

	//buffer->Apply();
	//sBuffer->SetConstantBuffer(buffer->Buffer());

	lineBuffer->Apply();
	sLineBuffer->SetConstantBuffer(lineBuffer->Buffer());

	shader->DrawIndexed(0, Pass(), indexCount);
}

void Terrain::BaseMap(wstring file)
{
	SafeDelete(baseMap);

	baseMap = new Texture(file);
}

void Terrain::LayerMap(wstring layer)
{
	SafeDelete(layerMap);
	SafeDelete(alphaMap);

	layerMap = new Texture(layer);
	alphaMap = new Texture(heightMap->GetFile());
}

void Terrain::SplattingLayerMap(wstring layer)
{
	SafeDelete(layerMap);

	splattingLayerMap[0] = new Texture(layer);
}

void Terrain::LayerMap(wstring layer, wstring alpha)
{
	SafeDelete(layerMap);
	SafeDelete(alphaMap);

	layerMap = new Texture(layer);
	alphaMap = new Texture(alpha);
}

void Terrain::HeightMap(wstring file)
{
	SafeDelete(heightMap);

	heightMap = new Texture(file);
}

float Terrain::GetHeight(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x > width - 1) 
		return FLT_MIN;
	if (z < 0 || z > height - 2) 
		return FLT_MIN;


	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + x + 1;
	index[3] = width * (z + 1) + x + 1;

	Vector3 v[4];
	for (int i = 0; i < 4; i++)
		v[i] = vertices[index[i]].Position;


	float ddx = (position.x - v[0].x) / 1.0f;
	float ddz = (position.z - v[0].z) / 1.0f;

	Vector3 result;

	if (ddx + ddz <= 1.0f)
	{
		result = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	}
	else
	{
		ddx = 1.0f - ddx;
		ddz = 1.0f - ddz;

		result = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}


	return result.y;
}

float Terrain::GetHeightPick(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x > width) return FLT_MIN;
	if (z < 0 || z > height) return FLT_MIN;


	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + x + 1;
	index[3] = width * (z + 1) + x + 1;

	Vector3 p[4];
	for (int i = 0; i < 4; i++)
		p[i] = vertices[index[i]].Position;


	Vector3 start(position.x, 1000, position.z);
	Vector3 direction(0, -1, 0);

	float u, v, distance;
	Vector3 result(-1, FLT_MIN, -1);

	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;

	return result.y;
}

Vector3 Terrain::GetPickedPosition()
{
	Matrix V = Context::Get()->View();
	Matrix P = Context::Get()->Projection();
	Viewport* Vp = Context::Get()->GetViewport();

	Vector3 mouse = Mouse::Get()->GetPosition();

	
	Vector3 n, f;
	mouse.z = 0.0f;
	Vp->Unproject(&n, mouse, transform->World(), V, P);

	mouse.z = 1.0f;
	Vp->Unproject(&f, mouse, transform->World(), V, P);

	Vector3 start = n;
	Vector3 direction = f - n;

	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			UINT index[4];
			index[0] = width * z + x;
			index[1] = width * (z + 1) + x;
			index[2] = width * z + x + 1;
			index[3] = width * (z + 1) + x + 1;

			Vector3 p[4];
			for (int i = 0; i < 4; i++)
				p[i] = vertices[index[i]].Position;


			float u, v, distance;
			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
				return p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

			if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
				return p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
		}
	}	

	return Vector3(-1, FLT_MIN, -1);
}

void Terrain::ReadTextreData(wstring imageFile)
{
	wstring ext = Path::GetExtension(imageFile);
	std::transform(ext.begin(), ext.end(), ext.begin(), toupper);
}

void Terrain::CreateVertexData()
{
	vector<Color> heights;
	heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	width = heightMap->GetWidth();
	height = heightMap->GetHeight();


	vertexCount = width * height;
	vertices = new TerrainVertex[vertexCount];
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * z + x;
			UINT pixel = width * (height - 1 - z) + x;

			vertices[index].Position.x = (float)x;
			vertices[index].Position.y = (heights[pixel].r * 255.0f) / 5.0f;
			vertices[index].Position.z = (float)z;

			vertices[index].Uv.x = (float)x / (float)width;
			vertices[index].Uv.y = (float)(height - 1 - z) / (float)height;
		}
	}
}

void Terrain::CreateIndexData()
{
	indexCount = (width - 1) * (height - 1) * 6;
	indices = new UINT[indexCount];

	UINT index = 0;
	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			indices[index + 0] = width * z + x;
			indices[index + 1] = width * (z + 1) + x;
			indices[index + 2] = width * z + x + 1;
			indices[index + 3] = width * z + x + 1;
			indices[index + 4] = width * (z + 1) + x;
			indices[index + 5] = width * (z + 1) + x + 1;

			index += 6;
		}
	}
}

void Terrain::CreateNormalData()
{
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		TerrainVertex v0 = vertices[index0];
		TerrainVertex v1 = vertices[index1];
		TerrainVertex v2 = vertices[index2];

		//NormalVector
		{
			Vector3 d1 = v1.Position - v0.Position;
			Vector3 d2 = v2.Position - v0.Position;

			Vector3 normal;
			D3DXVec3Cross(&normal, &d1, &d2);

			vertices[index0].Normal += normal;
			vertices[index1].Normal += normal;
			vertices[index2].Normal += normal;
		}
		
		//TangentVector
		{
			Vector3 d1 = v1.Position - v0.Position;
			Vector3 d2 = v2.Position - v0.Position;

			Vector2 u = v1.Uv - v0.Uv;
			Vector2 v = v2.Uv - v0.Uv;

			float d = 1.0f / (u.x * v.y - u.y * v.x);

			Vector3 tangent;
			tangent.x = (v.y * d1.x - v.x * d2.x) * d;
			tangent.y = (v.y * d1.y - v.x * d2.y) * d;
			tangent.z = (v.y * d1.z - v.x * d2.z) * d;

			vertices[index0].Tangent += tangent;
			vertices[index1].Tangent += tangent;
			vertices[index2].Tangent += tangent;
		}
	}

	for (UINT i = 0; i < vertexCount; i++)
	{
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
		D3DXVec3Normalize(&vertices[i].Tangent, &vertices[i].Tangent);
	}
		
}

void Terrain::UpdateVertexData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, vertices, sizeof(Terrain::TerrainVertex) * vertexCount);
	}
	D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
}

void Terrain::SetVertexData()
{
	ID3D11Texture2D* tex = heightMap->GetTexture();
	D3D11_TEXTURE2D_DESC texDesc; tex->GetDesc(&texDesc);

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = texDesc.Width;
	desc.Height = texDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = texDesc.Format;
	desc.SampleDesc = texDesc.SampleDesc;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_STAGING;

	ID3D11Texture2D* tempTexture;
	Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &tempTexture));
	Check(D3DX11LoadTextureFromTexture(D3D::GetDC(), tex, NULL, tempTexture));

	UINT* colors = new UINT[desc.Width * desc.Height];
	for (int i = 0; i < desc.Width * desc.Height; i++)
		colors[i] = pixels[i];

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(tempTexture, 0, D3D11_MAP_WRITE, NULL, &subResource);
	{
		memcpy(subResource.pData, colors, sizeof(UINT) * desc.Width * desc.Height);
	}
	D3D::GetDC()->Unmap(tempTexture, 0);
}

void Terrain::SetTerrainData()
{
	CreateVertexData();
	CreateIndexData();
	CreateNormalData();
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, vertices, sizeof(TerrainVertex) * vertexCount);
	}
	D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
	//bufferDesc.TerrainCellSpaceU = 1.0f / (float)heightMap->GetWidth() - 1;
	//bufferDesc.TerrainCellSpaceV = 1.0f / (float)heightMap->GetHeight() - 1;
}

Layer::Layer(Shader* shader, wstring file, wstring sSRV, wstring sMap)
	: shader(shader)
{
	Map = new Texture(file);

	this->sSRV = shader->AsSRV(String::ToString(sSRV));
	this->sMap = shader->AsSRV(String::ToString(sMap));
}

Layer::~Layer()
{
	SafeDeleteArray(Data);
	SafeRelease(Texture2D);
	SafeRelease(SRV);
	SafeDelete(Map);
}

void Layer::Render()
{
	sSRV->SetResource(SRV);
	sMap->SetResource(Map->SRV());
}

void Layer::ReadData(wstring imageFile)
{
}
