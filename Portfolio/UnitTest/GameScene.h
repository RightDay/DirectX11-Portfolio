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

	void Trees(UINT num);

private:
	CubeSky* sky;

	Shader* shader;
	Terrain* terrain;
	
	class Player* player;

	Vector3 playerPos = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 playerRot = Vector3(0.0f, 0.0f, 0.0f);

	class Mutant* mutant;
	class Warrok* warrok;
	ModelRender* trees = NULL;

	vector<MeshRender*> meshes;
	vector<ModelRender*> models;
	vector<ModelAnimator*> animators;

private:
	Shader* modelShader;
	Render2D* hpBar;
	Render2D* hpGauge;
};