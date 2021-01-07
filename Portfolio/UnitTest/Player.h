#pragma once
#include "GameActor.h"

class Player : public GameActor
{
private:
	enum PlayerState {STATE_IDLE, STATE_DANCE, STATE_JUMP, STATE_RUNNING};

public:
	Player();
	Player(ModelAnimator* model);
	~Player();

	void Update() override;
	void Render() override;

private:
	void CreateArcherModel();

	void Move();
	void Rotation();

	void playerControl();
	void playerMovePos(Vector3 pos, bool plus);
	void playerRotationAngle(float rotationAngle);

public:
	Vector3 GetPlayerPos() { return playerPos; }
	Vector3 GetPlayerRot() { return playerRot; }

private:
	ModelAnimator* model = NULL;
	ModelAnimator* archer = NULL;

	vector<ModelAnimator*> animators;

	Shader* modelShader = NULL;

private:
	Vector3 playerForward = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 playerRight = Vector3(0.0f, 0.0f, 0.0f);

	Vector3 playerPos = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 playerRot = Vector3(0.0f, 0.0f, 0.0f);

	Vector3 moveVertical = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 moveHorizontal = Vector3(0.0f, 0.0f, 0.0f);

	bool isMove = true;
	float velocity = 200.0f;
	float angle = 180.0f;
};