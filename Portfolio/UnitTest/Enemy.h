#pragma once
#include "GameActor.h"

class Enemy : public GameActor
{
public:
	Enemy();
	Enemy(ModelAnimator* model);
	~Enemy();

	void Update() override;
	void Update() override;
	void Render() override;

	void Jump() override;
	void Run() override;
	void Attack() override;

private:
	ModelAnimator* model = NULL;
};