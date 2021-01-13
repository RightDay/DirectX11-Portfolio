#include "stdafx.h"
#include "Player.h"
#include "PlayerState.h"

void StandingState::Enter(Player& player)
{
	player.model->PlayClip(0, Player::STATE_IDLE, 1.0f);
}

PlayerState* StandingState::handleInput(Player& player, Input input)
{
	if (input == PRESS_W || input == PRESS_S || input == PRESS_D || input == PRESS_A)
	{
		return new MovingState();
	}

	return NULL;
}

void StandingState::Update(Player& player)
{
}

void MovingState::Enter(Player& player)
{
	player.model->PlayClip(0, Player::STATE_RUN, 1.0f);
}

PlayerState* MovingState::handleInput(Player& player, Input input)
{
	if (input == PRESS_W)
	{
		player.playerMovePos(player.moveVertical, false);
	}
	if (input == PRESS_S)
	{
		player.playerMovePos(player.moveVertical, true);
	}
	if (input == PRESS_D)
	{
		player.playerMovePos(player.moveHorizontal, false);
	}
	if (input == PRESS_A)
	{
		player.playerMovePos(player.moveHorizontal, true);
	}
	else if (input == RELEASE_MOVE)
	{
		return new StandingState();
	}

	return NULL;
}

void MovingState::Update(Player& player)
{
}