#pragma once
#include "GameActor.h"

#define ENEMY_NUM 1

class Enemy : public GameActor
{
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

	void moveForward(UINT instance);
	void rotateAccordingToDistance(UINT instance, float distance);
	void rotateToPlayer(int instance, ModelAnimator* target);

private:
	ModelAnimator* model = NULL;

	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	float velocity = 100.0f;
	Vector3 forward = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 moveVertical = Vector3(0.0f, 0.0f, 0.0f);

	bool isRender = false;
};