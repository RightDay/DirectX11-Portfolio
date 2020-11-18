#pragma once

class Player
{
public:
	Player();
	Player(ModelAnimator* model);
	~Player();

	void Update();
	void Render();

private:
	void CreateArcherModel();

private:
	ModelAnimator* model = NULL;
	ModelAnimator* archer = NULL;

	vector<ModelAnimator*> animators;

	Shader* modelShader = NULL;
};