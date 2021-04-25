#pragma once

class Brush
{
public:
	Brush(Shader* shader, Terrain* terrain);
	~Brush();

	void Update();
	void Render();

private:

private:
	struct BrushDesc
	{
		Color Color = D3DXCOLOR(1, 0, 0, 1);
		Vector3 Location;
		UINT Type = 0;
		UINT Range = 1;
		float Padding[3];
	} brushDesc;


private:
	void RaiseHeight(UINT type, float intensity, bool bRaise);
	void FlatHeight(UINT type, float targetHeight);
	void SmoothHeight(UINT type);
	void Paint(UINT type, UINT layer);
	void AddModel(UINT num);
	void Splatting();

	void GetTerrainList();

	void OpenMesh();
	void LoadMesh(wstring file);

private:
	Vector3 location;

private:
	Shader* shader;
	Terrain* terrain;

	ConstantBuffer* brushBuffer;
	ID3DX11EffectConstantBuffer* sBrushBuffer;

	vector<wstring> terrainFiles;
	vector<Texture*> terrainTextures;

	vector<wstring> modelFileList;

	float flatHeight = 0.0f;

	int brushOption = 0;
};