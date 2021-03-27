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
	if (input == DOWN_W || input == DOWN_S || input == DOWN_D || input == DOWN_A)
	{
		PlayerState* state = new MovingState(); 

		return new MovingState(player, input);
	}
	
	if (input == ATTACK)
	{
		return new AttackState();
	}

	if (input == DASH_ATTACK)
	{
		return new DashAttackState();
	}

	return NULL;
}

void StandingState::Update(Player& player)
{
}

MovingState::MovingState(Player player, Input input)
{
	handleInput(player, input);
}

void MovingState::Enter(Player& player)
{
	player.bAttack = false;
	
	player.GetModel()->PlayClip(0, Player::STATE_RUN, 1.5f, 0.2f);
}

PlayerState* MovingState::handleInput(Player& player, Input input)
{
	//Axis Vertical
	if (input == DOWN_W)
	{ 
		inputDir.y = -1;
		//bMove = false;
	}

	if (input == DOWN_S)
	{
		inputDir.y = 1;
	}

	//Axis Horizontal
	if (input == DOWN_D)
	{
		inputDir.x = -1;
	}

	if (input == DOWN_A)
	{
		inputDir.x = 1;
	}
	
	//
	//Up
	//
	if (input == UP_W)
	{
		inputDir.y = 0;
	}

	if (input == UP_S)
	{
		inputDir.y = 0;
	}

	//Axis Horizontal
	if (input == UP_D)
	{
		inputDir.x = 0;
	}

	if (input == UP_A)
	{
		inputDir.x = 0;
	}

	if (abs(inputDir.x) < 1 && abs(inputDir.y) < 1)
	{
		return new StandingState();
	}

	angle = atan2(inputDir.x, inputDir.y);
	player.RotationAngle(angle);

	if (input == DASH_ATTACK)
	{
		return new DashAttackState();
	}

	return NULL;
}

void MovingState::Update(Player& player)
{
	player.playerMovePos(player.moveVertical, false);

	ImGui::Begin("MovingState");
	ImGui::Text("bMove : %d", bMove);
	ImGui::Text("InputDir : %f, %f", inputDir.x, inputDir.y);
	ImGui::End();
}

void MovingState::CalcurateDirection()
{
	if (abs(inputDir.x) < 1 && abs(inputDir.y) < 1)
		bMove = true;
}

void MovingState::Rotate(Player player)
{
	angle = atan2(inputDir.x, inputDir.y);
	player.RotationAngle(angle);
}

void MovingState::Move(Player player)
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
	if (input == DOWN_W || input == DOWN_S || input == DOWN_D || input == DOWN_A)
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
	if (player.GetModel()->StopAnim(0, 50))
	{
		player.bAttack = true;
	}

	if (player.GetModel()->StopAnim(0, 20))
	{
		player.bAttack = false;
		attackStop = true;
	}
}

void DashAttackState::Enter(Player& player)
{
	distance = 0.0f;

	player.GetModel()->PlayClip(0, Player::STATE_DASH_ATTACK, 4.0f, 0.1f);
}

PlayerState* DashAttackState::handleInput(Player& player, Input input)
{
	if (player.bAttack) return NULL;

	if (input == DOWN_W || input == DOWN_S || input == DOWN_D || input == DOWN_A)
	{
		PlayerState* state = new MovingState();

		return new MovingState(player, input);
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

void DashAttackState::Update(Player& player)
{
	//if (distance <= 300.0f)
	//{
	//	distance++;
	//	player.playerMovePos(player.moveVertical * 2.0f, false);
	//}

	if (player.bAttack)
	{
		player.playerMovePos(player.moveVertical * 2.0f, false);
	}

	if (player.GetModel()->StopAnim(0, 70))
	{
		player.bAttack = true;
	}

	if (player.GetModel()->StopAnim(0, 20))
	{
		player.bAttack = false;
		attackStop = true;
	}
}