#pragma once
#include "Systems/IExecute.h"

class GameScene : public IExecute
{
public:
	virtual void Initialize( ) override;
	virtual void Ready() override {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {};
	virtual void Render() override;
	virtual void PostRender() override {}
	virtual void ResizeScreen() override {}

private:
	void Pass(UINT mesh, UINT model, UINT anim);

private:
	CubeSky* sky;

	Shader* shader;
	TerrainLod* terrain;
	
	class Player* player;

	//vector<MeshRender*> meshes;
	//vector<ModelRender*> models;
	//vector<ModelAnimator*> animators;
};