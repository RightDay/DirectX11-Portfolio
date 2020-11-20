#include "Framework.h"
#include "TerrainLod.h"

TerrainLod::TerrainLod(InitializeDesc & desc)
	: Renderer(desc.shader), initDesc(desc)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	sBaseMap = shader->AsSRV("BaseMap");
	sLayerMap = shader->AsSRV("LayerMap");
	sAlphaMap = shader->AsSRV("AlphaMap");
	sHeightMap = shader->AsSRV("HeightMap");
	sNormalMap = shader->AsSRV("NormalMap");

	buffer = new ConstantBuffer(&bufferDesc, sizeof(BufferDesc));
	sBuffer = shader->AsConstantBuffer("CB_TerrainLod");


	heightMap = new Texture(initDesc.heightMap);
	sHeightMap->SetResource(heightMap->SRV());
	heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UINT, &heightMapPixel);

	width = this->heightMap->GetWidth() - 1;
	height = this->heightMap->GetHeight() - 1;


	vertexPerPatchX = (width / initDesc.CellsPerPatch) + 1;
	vertexPerPatchZ = (height / initDesc.CellsPerPatch) + 1;

	vertexCount = vertexPerPatchX * vertexPerPatchZ;
	faceCount = (vertexPerPatchX - 1) * (vertexPerPatchZ - 1);
	indexCount = faceCount * 4;


	CalcBoundY();
	CreateVertexData();
	CreateIndexData();

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexTerrain));
	indexBuffer = new IndexBuffer(indices, indexCount);

	bufferDesc.TexelCellSpaceU = 1.0f / (float)heightMap->GetWidth() - 1;
	bufferDesc.TexelCellSpaceV = 1.0f / (float)heightMap->GetHeight() - 1;
	bufferDesc.HeightRatio = initDesc.HeightRatio;

	camera = new Fixity();
	perspective = new Perspective(D3D::Width(), D3D::Height(), 0.1f, 1000.0f, Math::PI * 0.35f);

	frustum = new Frustum(NULL, perspective);
	//frustum = new Frustum(camera, perspective);
}

TerrainLod::~TerrainLod()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(buffer);
	SafeDelete(frustum);
	SafeDelete(camera);
	SafeDelete(perspective);

	SafeDelete(heightMap);

	SafeDelete(baseMap);
	SafeDelete(layerMap);
	SafeDelete(alphaMap);
}

void TerrainLod::Update()
{
	Super::Update();

	ImGui::SliderFloat("MinDistance", &bufferDesc.MinDistance, 1, 100);
	ImGui::SliderFloat("MaxDistance", &bufferDesc.MaxDistance, 1, 1000);

	camera->Update();

	frustum->Update();
	frustum->Planes(bufferDesc.WorldFrustumPlanes);
}

void TerrainLod::Render()
{
	Super::Render();

	if (baseMap != NULL)
		sBaseMap->SetResource(baseMap->SRV());

	if (layerMap != NULL)
		sLayerMap->SetResource(layerMap->SRV());

	if (alphaMap != NULL)
		sAlphaMap->SetResource(alphaMap->SRV());

	if (normalMap != NULL)
		sNormalMap->SetResource(normalMap->SRV());


	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
	shader->DrawIndexed(0, Pass(), indexCount);

}

void TerrainLod::BaseMap(wstring file)
{
	SafeDelete(baseMap);

	baseMap = new Texture(file);
}

void TerrainLod::LayerMap(wstring layer, wstring alpha)
{
	SafeDelete(alphaMap);
	SafeDelete(layerMap);

	alphaMap = new Texture(alpha);
	layerMap = new Texture(layer);
}

void TerrainLod::NormalMap(wstring file)
{
	SafeDelete(normalMap);

	normalMap = new Texture(file);
}

bool TerrainLod::InBounds(UINT x, UINT z)
{
	return x >= 0 && x < width && z >= 0 && z < height;
}

void TerrainLod::CalcPatchBounds(UINT x, UINT z)
{
	UINT x0 = x * initDesc.CellsPerPatch;
	UINT x1 = (x + 1) * initDesc.CellsPerPatch;

	UINT z0 = z * initDesc.CellsPerPatch;
	UINT z1 = (z + 1) * initDesc.CellsPerPatch;


	float minY = FLT_MAX;
	float maxY = FLT_MIN;

	for (UINT z = z0; z <= z1; z++)
	{
		for (UINT x = x0; x <= x1; x++)
		{
			float y = 0.0f;
			UINT pixel = width * (height - 1 - z) + x;

			if (InBounds(x, z))
				y = heightMapPixel[pixel].b * 255 / initDesc.HeightRatio;

			minY = min(minY, y);
			maxY = max(maxY, y);
		}
	}

	UINT patchID = (vertexPerPatchX - 1) * z + x;
	bounds[patchID] = Vector2(minY, maxY);
}

void TerrainLod::CalcBoundY()
{
	bounds.assign(faceCount, Vector2());

	for (UINT z = 0; z < vertexPerPatchZ - 1; z++)
	{
		for (UINT x = 0; x < vertexPerPatchX - 1; x++)
		{
			CalcPatchBounds(x, z);
		}
	}
}

void TerrainLod::CreateVertexData()
{
	vertices = new VertexTerrain[vertexCount];

	float halfWidth = (float)width * 0.5f;
	float halfHeight = (float)height * 0.5f;

	float patchWidth = (float)width / (float)(vertexPerPatchX - 1);
	float patchHeight = (float)height / (float)(vertexPerPatchZ - 1);

	float u = 1.0f / (float)(vertexPerPatchX - 1);
	float v = 1.0f / (float)(vertexPerPatchZ - 1);

	for (UINT z = 0; z < vertexPerPatchZ; z++)
	{
		float z1 = halfHeight - (float)z * patchHeight;

		for (UINT x = 0; x < vertexPerPatchX; x++)
		{
			float x1 = -halfWidth + (float)x * patchWidth;
			UINT patchId = vertexPerPatchX * z + x;

			vertices[patchId].Position = Vector3(x1, 0, z1);
			vertices[patchId].Uv = Vector2(x * u, z * v);
		}
	}

	for (UINT z = 0; z < vertexPerPatchZ - 1; z++)
	{
		for (UINT x = 0; x < vertexPerPatchX - 1; x++)
		{
			UINT patchID = (vertexPerPatchX - 1) * z + x;
			UINT vertID = vertexPerPatchX * z + x;

			vertices[vertID].BoundsY = bounds[patchID];
		}
	}
}

void TerrainLod::CreateIndexData()
{
	UINT index = 0;
	this->indices = new UINT[indexCount];
	for (UINT z = 0; z < vertexPerPatchZ - 1; z++)
	{
		for (UINT x = 0; x < vertexPerPatchX - 1; x++)
		{
			this->indices[index++] = vertexPerPatchX * z + x;
			this->indices[index++] = vertexPerPatchX * z + x + 1;
			this->indices[index++] = vertexPerPatchX * (z + 1) + x;
			this->indices[index++] = vertexPerPatchX * (z + 1) + x + 1;
		}
	}
}