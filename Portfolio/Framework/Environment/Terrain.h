#pragma once

class Terrain : public Renderer
{
public:
	typedef VertexTextureNormalTangent TerrainVertex;

public:
	Terrain(Shader* shader, wstring heightFile);
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

	float GetHeight(Vector3& position);
	float GetHeightPick(Vector3& position);
	Vector3 GetPickedPosition();
	void RaiseHeight(Vector3& position, UINT type, UINT range);

private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData();

private:
	struct BufferDesc
	{
		float TerrainCellSpaceU;
		float TerrainCellSpaceV;
		float WorldCellSpace = 1.0f;
		float HeightRatio = 1.0f;
	} bufferDesc;

	struct BrushDesc
	{
		Color Color = D3DXCOLOR(0, 1, 0, 1);
		Vector3 Location;
		UINT Type = 0;
		UINT Range = 1;
		float Padding[3];
	} brushDesc;

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



	ConstantBuffer* brushBuffer;
	ID3DX11EffectConstantBuffer* sBrushBuffer;

	ConstantBuffer* lineBuffer;
	ID3DX11EffectConstantBuffer* sLineBuffer;
};