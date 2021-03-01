#include "stdafx.h"
#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::Enemy(ModelAnimator* model)
	:model(model)
{
	
}

Enemy::~Enemy()
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

void Enemy::Move()
{
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

void Enemy::CreateModel()
{
}
