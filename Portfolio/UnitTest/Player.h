#pragma once

class Player
{
private:
	enum PlayerState {STATE_IDLE, STATE_DANCE, STATE_JUMP, STATE_RUNNING};

public:
	Player();
	Player(ModelAnimator* model);
	~Player();

	void Update();
	void Render();

private:
	void CreateArcherModel();
	void Move();

public:
	Vector3 GetPlayerPos() { return playerPos; }

private:
	ModelAnimator* model = NULL;
	ModelAnimator* archer = NULL;

	vector<ModelAnimator*> animators;

	Shader* modelShader = NULL;

private:
	Vector3 playerForward = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 playerRight = Vector3(0.0f, 0.0f, 0.0f);

	Vector3 playerPos = Vector3(0.0f, 0.0f, 0.0f);

	Vector3 moveVertical = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 moveHorizontal = Vector3(0.0f, 0.0f, 0.0f);

	bool isMove = false;
	float velocity = 200.0f;
};