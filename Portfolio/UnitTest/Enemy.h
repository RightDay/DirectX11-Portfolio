#pragma once
#include "GameActor.h"

#define ENEMY_NUM 3


class Enemy : public GameActor
{
public:
	enum eAnimState { E_STATE_RUNNING, E_STATE_ATTACK, E_STATE_DYING, E_STATE_WALKING};

public:
	Enemy();
	Enemy(ModelAnimator* model);
	~Enemy();

	void Initialize(ModelAnimator* model);
	void Update() override;
	void Render() override;

	void Move() override;
	void Jump() override;
	void Run() override;
	void Attack() override;
	
public:
	ModelAnimator* GetModel() { return model; }
	void Patrol(ModelAnimator* target);
	bool IsIntersect(ColliderObjectDesc* other, UINT instance);

private:
	void CreateModel();
	void handleState(eAnimState returnState, UINT instance);

public:
	void CreateCollider(UINT instance, ColliderObjectDesc*& collider);
	void CreateAttackCollider(ColliderObjectDesc*& collider, UINT instance, Vector3 srt[3]);

	void AttachCollider();
	void AttachAttackCollider(ColliderObjectDesc*& collider, UINT instance, UINT attachCollider);

	void moveForward(UINT instance);
	void rotateAccordingToDistance(UINT instance, float distance);
	void rotateToPlayer(int instance, ModelAnimator* target);

private:
	ModelAnimator* model = NULL;

	class EnemyState* state[ENEMY_NUM];

	
	//ColliderObjectDesc* attackCollider;

	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	float velocity = 100.0f;
	Vector3 forward = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 moveVertical = Vector3(0.0f, 0.0f, 0.0f);

	bool isRender = false;

public:
	ColliderObjectDesc* collider[ENEMY_NUM];
	ColliderObjectDesc* attackCollider[ENEMY_NUM];

	Vector3 attackColliderSRT[3];

	bool isLive[ENEMY_NUM];
	bool bAttack[ENEMY_NUM];

	UINT hp[ENEMY_NUM];
};