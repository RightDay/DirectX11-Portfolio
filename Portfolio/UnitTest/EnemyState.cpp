#include "stdafx.h"
#include "Enemy.h"
#include "EnemyState.h"

void E_MovingState::Enter(Enemy& enemy, UINT instance)
{
	enemy.bMove[instance] = true;
	enemy.bRotate[instance] = true;
	enemy.bAttack[instance] = false;
	enemy.GetModel()->PlayClip(instance, Enemy::E_STATE_RUNNING, 1.0f);
}

EnemyState* E_MovingState::handleState(Enemy::eAnimState animState)
{
	if (animState == Enemy::E_STATE_ATTACK)
	{
		return new E_AttackState();
	}

	if (animState == Enemy::E_STATE_DYING)
	{
		return new E_DyingState();
	}

	return NULL;
}

void E_MovingState::Update(Enemy& enemy, UINT instance)
{
	enemy.moveForward(instance);
	enemy.rotateAccordingToDistance(instance, 100.0f);
}

void E_AttackState::Enter(Enemy& enemy, UINT instance)
{
	enemy.bMove[instance] = false;
	enemy.bRotate[instance] = false;
	enemy.bAttack[instance] = false;
	enemy.GetModel()->PlayClip(instance, Enemy::E_STATE_ATTACK, 1.0f);
}

EnemyState* E_AttackState::handleState(Enemy::eAnimState animState)
{
	if (animState == Enemy::E_STATE_RUNNING)
	{
		return new E_MovingState();
	}

	if (animState == Enemy::E_STATE_DYING)
	{
		return new E_DyingState();
	}

	return NULL;
}

void E_AttackState::Update(Enemy& enemy, UINT instance)
{
	if (enemy.GetModel()->StopAnim(instance, 50))
	{
		enemy.bMove[instance] = false;
		enemy.bRotate[instance] = false;
		enemy.bAttack[instance] = true;
	}
	if (enemy.GetModel()->StopAnim(instance, 20))
	{
		enemy.bAttack[instance] = false;
	}
	if (enemy.GetModel()->StopAnim(instance, 1))
	{
		enemy.bMove[instance] = true;
		enemy.bRotate[instance] = true;
	}
}

void E_DyingState::Enter(Enemy& enemy, UINT instance)
{
	enemy.GetModel()->PlayClip(instance, Enemy::E_STATE_DYING, 2.0f);
}

EnemyState* E_DyingState::handleState(Enemy::eAnimState animState)
{
	return nullptr;
}

void E_DyingState::Update(Enemy& enemy, UINT instance)
{
	if (enemy.GetModel()->StopAnim(instance, 10))
	{
		enemy.isLive[instance] = false;
	}
	if (enemy.isLive[instance] == false)
	{
		enemy.GetModel()->GetTransform(instance)->Position(-3000, -3000, -3000);
	}
}
