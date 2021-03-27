#include "stdafx.h"
#include "Enemy.h"
#include "EnemyState.h"

void E_MovingState::Enter(Enemy& enemy, UINT instance)
{
	enemy.bMove = true;
	enemy.bRotate = true;
	enemy.bAttack[instance] = false;
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
	enemy.bMove = false;
	enemy.bRotate = false;
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

	if (animState == Enemy::E_STATE_DYING)
	{
		return new E_DyingState();
	}

	return NULL;
}

void E_AttackState::Update(Enemy& enemy, UINT instance)
{
	if (enemy.GetModel()->StopAnim(instance, 60))
	{
		enemy.bMove = false;
		enemy.bRotate = false;
		enemy.bAttack[instance] = true;
	}
	if (enemy.GetModel()->StopAnim(instance, 20))
	{
		enemy.bAttack[instance] = false;
	}
	if (enemy.GetModel()->StopAnim(instance, 1))
	{
		enemy.bMove = true;
		enemy.bRotate = true;
	}

	ImGui::Begin("AttackState");
	ImGui::Text("Attack : %d", attack);
	ImGui::Text("bAttack[%d] : %d", instance, enemy.bAttack[instance]);
	ImGui::End();
}

void E_DyingState::Enter(Enemy& enemy, UINT instance)
{
	enemy.GetModel()->PlayClip(instance, Enemy::E_STATE_DYING, 1.5f);
}

EnemyState* E_DyingState::handleState(Enemy::eAnimState animState)
{
	return nullptr;
}

void E_DyingState::Update(Enemy& enemy, UINT instance)
{
	if (enemy.GetModel()->StopAnim(instance, 1))
	{
		enemy.bAttack[instance] = true;
		enemy.isLive[instance] = false;
	}
	if (enemy.isLive[instance] == false)
	{
		enemy.GetModel()->GetTransform(instance)->Position(-300, -300, -300);
	}
}
