#include "stdafx.h"
#include "Enemy.h"
#include "EnemyState.h"

//void MovingState::Enter(Enemy& enemy, UINT instance)
//{
//	enemy.GetModel()->PlayClip(instance, Enemy::E_STATE_RUNNING, 1.0f);
//}

void MovingState::Enter(ModelAnimator* model, UINT instance)
{
	model->PlayClip(instance, Enemy::E_STATE_RUNNING, 1.0f);
}

EnemyState* MovingState::handleState(Enemy::eAnimState animState)
{
	if (animState == Enemy::E_STATE_ATTACK)
	{
		return new AttackState();
	}
}

void MovingState::Update(Enemy& Enemy)
{
}

//void AttackState::Enter(Enemy& enemy, UINT instance)
//{
//	enemy.GetModel()->PlayClip(instance, Enemy::E_STATE_ATTACK, 1.0f);
//}

void AttackState::Enter(ModelAnimator* model, UINT instance)
{
	model->PlayClip(instance, Enemy::E_STATE_ATTACK, 1.0f);
}

EnemyState* AttackState::handleState(Enemy::eAnimState animState)
{
	if (animState == Enemy::E_STATE_RUNNING)
	{
		return new MovingState();
	}
}

void AttackState::Update(Enemy& Enemy)
{
}
