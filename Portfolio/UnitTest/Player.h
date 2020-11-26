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
	void Move();

private:
	ModelAnimator* model = NULL;
	ModelAnimator* archer = NULL;

	vector<ModelAnimator*> animators;

	Shader* modelShader = NULL;

private:
	Vector3 playerForward = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 playerPos = Vector3(0.0f, 0.0f, 0.0f);
};