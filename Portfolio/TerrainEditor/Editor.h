#pragma once
#include "Systems/IExecute.h"

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
	void ImportBaseMap(wstring files);
	void ImportHeightMap(wstring files);

private:
	CubeSky* sky;

	Shader* shader;
	TerrainLod* terrain;

	Texture* baseMapTexture;
	Texture* heightMapTexture;
};