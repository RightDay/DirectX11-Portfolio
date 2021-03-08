#include "stdafx.h"
#include "Enemy.h"
#include "EnemyState.h"

void E_MovingState::Enter(Enemy& enemy, UINT instance)
{
	enemy.GetModel()->PlayClip(instance, Enemy::E_STATE_RUNNING, 1.0f);
}

//void E_MovingState::Enter(ModelAnimator* model, UINT instance)
//{
//	model->PlayClip(instance, Enemy::E_STATE_RUNNING, 1.0f);
//}

EnemyState* E_MovingState::handleState(Enemy::eAnimState animState)
{
	if (animState == Enemy::E_STATE_ATTACK)
	{
		return new E_AttackState();
	}

	return NULL;
}

void E_MovingState::Update(Enemy& enemy, UINT instance)
{
	enemy.moveForward(instance);
	enemy.rotateAccordingToDistance(instance, 40.0f);
}

void E_AttackState::Enter(Enemy& enemy, UINT instance)
{
	enemy.GetModel()->PlayClip(instance, Enemy::E_STATE_ATTACK, 1.0f);
}

//void E_AttackState::Enter(ModelAnimator* model, UINT instance)
//{
//	model->PlayClip(instance, Enemy::E_STATE_ATTACK, 1.0f);
//}

EnemyState* E_AttackState::handleState(Enemy::eAnimState animState)
{
	if (animState == Enemy::E_STATE_RUNNING)
	{
		return new E_MovingState();
	}

	return NULL;
}

void E_AttackState::Update(Enemy& enemy, UINT instance)
{
}