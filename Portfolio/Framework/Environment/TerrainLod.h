#pragma once

class TerrainLod : public Renderer
{
public:
	struct InitializeDesc;

public:
	TerrainLod(InitializeDesc& desc);
	~TerrainLod();

	void Update();
	void Render();

	Texture* BaseMap() { return baseMap; }
	void BaseMap(wstring file);
	void SetBaseMap() { sBaseMap->SetResource(baseMap->SRV()); }

	Texture* LayerMap() { return layerMap; }
	void LayerMap(wstring layer);
	void LayerMap(wstring layer, wstring alpha);

	Texture* NormalMap() { return normalMap; }
	void NormalMap(wstring file);
	void SetNormalMap() { sNormalMap->SetResource(normalMap->SRV()); }

	Texture* HeightMap() { return heightMap; }
	void HeightMap(wstring file);
	void SetHeightMap() { sHeightMap->SetResource(heightMap->SRV()); }

	float GetHeight(Vector3& position);

private:
	bool InBounds(UINT x, UINT z);
	void CalcPatchBounds(UINT x, UINT z);
	void CalcBoundY();

	void CreateVertexData();
	void CreateIndexData();

public:
	struct InitializeDesc
	{
		Shader* shader;

		wstring heightMap;
		float CellSpacing;
		UINT CellsPerPatch;
		float HeightRatio;
	};

private:
	struct BufferDesc
	{
		float MinDistance = 1.0f;
		float MaxDistance = 500.0f;
		float MinTessellation = 1.0f;
		float MaxTessellation = 64.0f;

		float TexelCellSpaceU;
		float TexelCellSpaceV;
		float WorldCellSpace = 1.0f;
		float HeightRatio = 1.0f;

		Plane WorldFrustumPlanes[6];
	} bufferDesc;

private:
	struct VertexTerrain
	{
		Vector3 Position;
		Vector2 Uv;
		Vector2 BoundsY;
	};

private:
	UINT faceCount;

	UINT vertexPerPatchX;
	UINT vertexPerPatchZ;

private:
	InitializeDesc initDesc;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	UINT width, height;

	VertexTerrain* vertices;
	UINT* indices;

	Frustum* frustum;
	Camera* camera;
	Perspective* perspective;

	Texture* heightMap = NULL;
	vector<D3DXCOLOR> heightMapPixel;
	ID3DX11EffectShaderResourceVariable* sHeightMap;

	vector<Vector2> bounds;

	Texture* baseMap = NULL;
	ID3DX11EffectShaderResourceVariable* sBaseMap;

	Texture* layerMap = NULL;
	ID3DX11EffectShaderResourceVariable* sLayerMap;

	Texture* alphaMap = NULL;
	ID3DX11EffectShaderResourceVariable* sAlphaMap;

	Texture* normalMap = NULL;
	ID3DX11EffectShaderResourceVariable* sNormalMap;
};