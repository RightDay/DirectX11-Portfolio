#pragma once

class Terrain : public Renderer
{
private:
	struct TerrainVertex
	{
		Vector3 Position = Vector3(0, 0, 0);
		Vector2 Uv = Vector3(0, 0, 0);
		Vector3 Normal = Vector3(0, 0, 0);
		Vector3 Tangent = Vector3(0, 0, 0);
		Vector3 Color = Vector3(0, 0, 0);
	};

public:
	Terrain(Shader* shader, wstring heightFile);
	Terrain(Shader* shader, wstring file, bool bDDS);
	~Terrain();

	void Update();
	void Render();

	Texture* BaseMap() { return baseMap; }
	void BaseMap(wstring file);
	void SetBaseMap() { sBaseMap->SetResource(baseMap->SRV()); }

	Texture* LayerMap() { return layerMap; }
	void LayerMap(wstring layer);
	void LayerMap(wstring layer, wstring alpha);
	void SetLayerMap() { sLayerMap->SetResource(layerMap->SRV()); }

	Texture* HeightMap() { return heightMap; }
	void HeightMap(wstring file);
	void SetHeightMap() { sHeightMap->SetResource(heightMap->SRV()); }

	Texture* splattingLayerMap[2];
	ID3DX11EffectShaderResourceVariable* sSplattingLayerMap[2];
	void SplattingLayerMap(wstring layer);
	void SetSplattingLayerMap() { sSplattingLayerMap[0]->SetResource(splattingLayerMap[0]->SRV()); }

	float GetHeight(Vector3& position);
	float GetHeightPick(Vector3& position);
	Vector3 GetPickedPosition();

	UINT GetWidth() { return width; }
	UINT GetHeight() { return height; }

	TerrainVertex* Vertices() { return vertices; }
	//void SetVerticesY(UINT index, float y) { vertices[index].Position.y += y; }
	void SetLayer1(wstring file);
	void ReadTextureData(wstring imageFile);

public:
	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData();

public:
	void UpdateVertexData();
	void SetVertexData();

public:
	void SetTerrainData();

private:
	//struct BufferDesc
	//{
	//	float TerrainCellSpaceU;
	//	float TerrainCellSpaceV;
	//	float WorldCellSpace = 1.0f;
	//	float HeightRatio = 1.0f;
	//} bufferDesc;

	struct LineDesc
	{
		Color Color = D3DXCOLOR(1, 1, 1, 1);
		UINT Visible = 1;
		float Thickness = 0.01f;
		float Size = 5.0f;
		float Padding;
	} lineDesc;

private:
	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	Texture* heightMap = NULL;
	ID3DX11EffectShaderResourceVariable* sHeightMap;

	UINT width;
	UINT height;

	TerrainVertex* vertices;	
	UINT* indices;

	Texture* baseMap = NULL;
	ID3DX11EffectShaderResourceVariable* sBaseMap;

	Texture* layerMap = NULL;
	ID3DX11EffectShaderResourceVariable* sLayerMap;

	Texture* alphaMap = NULL;
	ID3DX11EffectShaderResourceVariable* sAlphaMap;

	ConstantBuffer* lineBuffer;
	ID3DX11EffectConstantBuffer* sLineBuffer;

	wstring imageFile;

public:
	Color* pixels;
};

class Layer
{
public:
	Layer(Shader* shader, wstring file, wstring sSRV, wstring sMap);
	~Layer();

	void Render();
	void ReadData(wstring imageFile);

private:
	wstring file = L"";
	float* Data = NULL;
	Shader* shader;

	ID3D11Texture2D* Texture2D = NULL;
	ID3D11ShaderResourceView* SRV = NULL;
	ID3DX11EffectShaderResourceVariable* sSRV;

	Texture* Map = NULL;
	ID3DX11EffectShaderResourceVariable* sMap;
};