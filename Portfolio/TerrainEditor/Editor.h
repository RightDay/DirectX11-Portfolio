#pragma once
#include "Systems/IExecute.h"

enum class MapTypes { BASE_MAP, LAYER_MAP, NORMAL_MAP, HEIGHT_MAP };
enum FillModes { NONE = 0, WIRE_FRAME };
class Editor : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {}
	virtual void Render() override;
	virtual void PostRender() override {}
	virtual void ResizeScreen() override {}

private:
	Texture * GetMapTexture(MapTypes mapTypes);

	void AddSettingTypeCombobox();
	void NoneToolType();
	void HeightMapToolType();
	void DetailTerrainToolType();
	void TerrainBrushToolType();

	void ImportBaseMap(wstring files);
	void ImportLayerMap(wstring files);
	void ImportHeightMap(wstring files);

	function<void(wstring)> GetImportTextureMapFunction(function<void(wstring)>& func, void(Editor::* function)(wstring files));
	Texture* GetTextureMap(Texture *& mapTexture, MapTypes mapTypes);

	void AddMapButton(Texture * mapTexture, ImVec2 size, function<void(wstring)> func);

	void ChangeFillMode(int fillmode) { terrain->Pass(fillmode); }

	void AddBillboard();

private:
	bool showTerrainEditor = true;

private:
	CubeSky* sky;

	Shader* shader;
	Terrain* terrain;

	Texture* baseMapTexture = NULL;
	Texture* layerMapTexture = NULL;
	Texture* normalMapTexture = NULL;
	Texture* heightMapTexture = NULL;

	function<void(wstring)> func = NULL;
	ImVec2 btnSize = ImVec2(100.0f, 100.0f);

	Billboard* billboard;
	Shader* billboardShader;
};