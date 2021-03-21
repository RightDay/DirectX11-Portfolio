#include "stdafx.h"
#include "Player.h"
#include "PlayerState.h"

void StandingState::Enter(Player& player)
{
	player.bAttack = false;
	player.GetModel()->PlayClip(0, Player::STATE_IDLE, 1.0f, 0.5f);
}

PlayerState* StandingState::handleInput(Player& player, Input input)
{
	if (input == PRESS_W || input == PRESS_S || input == PRESS_D || input == PRESS_A)
	{
		return new MovingState();
	}
	
	if (input == ATTACK)
	{
		return new AttackState();
	}

	return NULL;
}

void StandingState::Update(Player& player)
{
}

void MovingState::Enter(Player& player)
{
	player.bAttack = false;
	player.GetModel()->PlayClip(0, Player::STATE_RUN, 1.5f, 0.2f);
}

PlayerState* MovingState::handleInput(Player& player, Input input)
{
	static Vector3 pForward = player.GetModel()->GetTransform(0)->Forward();
	if (input == PRESS_W)
	{
		pForward = player.GetModel()->GetTransform(0)->Forward();

		if (pForward.z <= -0.075 || pForward.z >= -0.074)
		{
			player.playerRotationAngle(-5.0f);
		}
	}

	if (input == PRESS_S)
	{
		pForward = player.GetModel()->GetTransform(0)->Forward();

		if (pForward.z >= 0.075 || pForward.z <= 0.074)
		{
			player.playerRotationAngle(+5.0f);
		}
	}

	if (input == PRESS_D)
	{
		pForward = player.GetModel()->GetTransform(0)->Forward();
		if (pForward.z > 0)
		{
			player.playerRotationAngle(-5.0f);
		}
	}

	if (input == PRESS_A)
	{
		pForward = player.GetModel()->GetTransform(0)->Forward();
		if (pForward.z< 0)
		{
			player.playerRotationAngle(-5.0f);
		}
	}

	else if (input == RELEASE_MOVE)
	{
		return new StandingState();
	}

	return NULL;
}

void MovingState::Update(Player& player)
{
	player.playerMovePos(player.moveVertical, false);
}

void AttackState::Enter(Player& player)
{
	player.bAttack = true;
	player.GetModel()->PlayClip(0, Player::STATE_ATTACK, 2.0f, 0.1f);
}

PlayerState* AttackState::handleInput(Player& player, Input input)
{
	if (input == PRESS_W || input == PRESS_S || input == PRESS_D || input == PRESS_A)
	{
		return new MovingState();
	}

	if (input == INPUT_NULL)
	{
		if (attackStop)
		{
			return new StandingState();
		}
		attackStop = false;
	}
	return NULL;
}

void AttackState::Update(Player& player)
{
	if (player.GetModel()->StopAnim(0, 20))
	{
		attackStop = true;
	}
}
