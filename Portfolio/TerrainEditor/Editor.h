#pragma once
#include "Systems/IExecute.h"

enum class MapTypes { BASE_MAP, LAYER_MAP, NORMAL_MAP, HEIGHT_MAP };
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
	void NoneSettingType();
	void HeightMapSettingType();

	void ImportBaseMap(wstring files);
	void ImportLayerMap(wstring files);
	void ImportNormalMap(wstring files);
	void ImportHeightMap(wstring files);
	void UpdateMapImage(Texture *& mapTexture, MapTypes mapTypes, function<void(wstring)>& func, void(Editor::* function)(wstring files));

	void AddMapButton(Texture * mapTexture, ImVec2 size, function<void(wstring)> func);

private:
	bool showTerrainEditor = true;

private:
	CubeSky* sky;

	Shader* shader;
	TerrainLod* terrain;

	Texture* baseMapTexture = NULL;
	Texture* layerMapTexture = NULL;
	Texture* normalMapTexture = NULL;
	Texture* heightMapTexture = NULL;

	function<void(wstring)> func = NULL;
	ImVec2 btnSize = ImVec2(100.0f, 100.0f);
};