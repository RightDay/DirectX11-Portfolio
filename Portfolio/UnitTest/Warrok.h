#pragma once
#include "Enemy.h"

enum W_Anim { W_WALK, W_ATTACK };

class Warrok : public Enemy
{
public:
	Warrok();
	Warrok(ModelAnimator* model);
	~Warrok();

	void Update() override;
	void Render() override;

	void Move() override;
	void Jump() override;
	void Run() override;
	void Attack() override;

public:
	ModelAnimator* GetModel() { return model; }
	void isIntersect(ColliderObjectDesc* other, UINT instance);

private:
	void CreateModel(UINT modelNum);

private:
	ModelAnimator* model = NULL;
	Shader* modelShader = NULL;

	//ColliderObjectDesc* attackCollider[ENEMY_NUM];

	UINT lefthandBoneNumber;
};