#pragma once
#include "GameActor.h"

enum Input { PRESS_W, PRESS_A, PRESS_D, PRESS_S, RELEASE_MOVE, ATTACK, INPUT_NULL };

class Player : public GameActor
{
public:
	enum AnimState { STATE_IDLE, STATE_RUN, STATE_ATTACK };

public:
	Player();
	Player(ModelAnimator* model);
	~Player();

	void Update() override;
	void Render() override;

	void Jump() override;
	void Run() override;
	void Attack() override;

	void handleInput(Input input);

private:
	void CreateArcherModel();
	void CreatePlayerCollider();
	void AttachCollider();
	void ControlCollider(ColliderObjectDesc* collider);

	void Move();
	void Rotation();

	void PlayerControl();

public:
	void playerMovePos(Vector3 pos, bool plus);
	void playerRotationAngle(float rotationAngle);

public:
	ModelAnimator* GetModel() { return model; }
	Vector3 * GetPlayerPos() { return &playerPos; }
	Vector3 GetPlayerRot() { return playerRot; }

private:
	ModelAnimator* model = NULL;
	ModelAnimator* archer = NULL;

	ColliderObjectDesc* playerCollider;
	ColliderObjectDesc* swordCollider;

	Model* weapon = NULL;

	Shader* modelShader = NULL;

private:
	class PlayerState* state;

	Vector3 playerForward = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 playerRight = Vector3(0.0f, 0.0f, 0.0f);

	Vector3 playerPos = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 playerRot = Vector3(0.0f, 0.0f, 0.0f);

	bool isMove = true;
	float velocity = 200.0f;
	float angle = 0.0f;

public:
	Vector3 moveVertical = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 moveHorizontal = Vector3(0.0f, 0.0f, 0.0f);
};