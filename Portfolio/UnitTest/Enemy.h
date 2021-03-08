#pragma once
#include "GameActor.h"

#define ENEMY_NUM 3


class Enemy : public GameActor
{
public:
	enum eAnimState { E_STATE_RUNNING, E_STATE_ATTACK, E_STATE_WALK};

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

private:
	void CreateModel();
	void handleState(eAnimState returnState, UINT instance);

public:
	void moveForward(UINT instance);
	void rotateAccordingToDistance(UINT instance, float distance);
	void rotateToPlayer(int instance, ModelAnimator* target);

private:
	ModelAnimator* model = NULL;

	class EnemyState* state[ENEMY_NUM];

	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	float velocity = 100.0f;
	Vector3 forward = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 moveVertical = Vector3(0.0f, 0.0f, 0.0f);

	bool isRender = false;
};