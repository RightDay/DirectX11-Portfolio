#include "stdafx.h"
#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::Enemy(ModelAnimator* model)
	:model(model)
{
	
}

void Enemy::Update()
{
	model->Update();
}

void Enemy::Render()
{
	model->Render();
}

void Enemy::Jump()
{
}

void Enemy::Run()
{
}

void Enemy::Attack()
{
}
