#pragma once
#include "GameActor.h"

enum Input { DOWN_W, DOWN_A, DOWN_D, DOWN_S, UP_W, UP_A, UP_D, UP_S, RELEASE_MOVE, ATTACK, INPUT_NULL };

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
	void CreateHpBar();

	void AttachCollider();
	void AttachSwordCollider();
	void ControlCollider(ColliderObjectDesc* collider);

	void Move();
	void Rotation();

	void PlayerControl();

	void controlHpBar();

public:
	void playerMovePos(Vector3 pos, bool plus);
	void RotationAngle(float angle);
	bool IsIntersect(ColliderObjectDesc* other);
	void playerGetHeight(Terrain* terrain);

public:
	ModelAnimator* GetModel() { return model; }
	Vector3 * GetPlayerPos() { return &playerPos; }
	Vector3 GetPlayerRot() { return playerRot; }
	float GetAngle() { return angle; }
	void SetAngle(float angle) { this->angle = angle; }

public:
	ColliderObjectDesc* playerCollider;
	ColliderObjectDesc* swordCollider;

	Vector3 moveVertical = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 moveHorizontal = Vector3(0.0f, 0.0f, 0.0f);

	bool bAttack;

private:
	ModelAnimator* model = NULL;
	ModelAnimator* archer = NULL;

	Model* weapon = NULL;

	Shader* modelShader = NULL;
	Shader* gridShader = NULL;
	class PlayerState* state;

	Vector3 playerForward = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 playerRight = Vector3(0.0f, 0.0f, 0.0f);

	Vector3 playerPos = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 playerRot = Vector3(0.0f, 0.0f, 0.0f);

	bool isMove = true;
	bool isRender = false;

	float velocity = 200.0f;
	float turnSpeed = 10.0f;
	float angle = 0.0f;

	UINT attachBone;

	Render2D* hpBar;
	Render2D* hpGauge;
};