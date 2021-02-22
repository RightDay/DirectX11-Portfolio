#include "stdafx.h"
#include "Player.h"
#include "PlayerState.h"

Player::Player()
{
	model = archer;
	playerPos = Vector3(0, 5.0f, 0);

	state = new StandingState();
}

Player::Player(ModelAnimator* model)
	:model(model)
{
	state = new StandingState();
}

Player::~Player()
{
}

void Player::Update()
{
	model->Update();

	state->Update(*this);

	PlayerControl();

	Move();
	Rotation();
}

void Player::Render()
{
	//Insert Shader Pass

	model->Render();
}

void Player::Jump()
{
}

void Player::Run()
{
}

void Player::Attack()
{
}

void Player::handleInput(Input input)
{
	PlayerState* state_ = state->handleInput(*this, input);
	if (state_ != NULL)
	{
		delete state;
		state = state_;

		state->Enter(*this);
	}
}

void Player::Move()
{
	if (Mouse::Get()->Down(1) == true) return;

	playerForward = model->GetTransform(0)->Forward();
	playerRight = model->GetTransform(0)->Right();
	
	moveVertical = playerForward * velocity * Time::Delta() * 2.0f;
	moveHorizontal = playerRight * velocity * Time::Delta() * 2.0f;

	if (isMove == false) return;

	model->GetTransform(0)->Position(playerPos);
	model->UpdateTransforms();
}

void Player::Rotation()
{
	playerRot = Vector3(0.0f, angle, 0.0f);

	model->GetTransform(0)->RotationDegree(playerRot);
}

void Player::PlayerControl()
{
	//Player move forward
	if (Keyboard::Get()->Press('W'))
	{
		handleInput(PRESS_W);
	}

	//Player move backward
	if (Keyboard::Get()->Press('S'))
	{
		handleInput(PRESS_S);
	}

	//Player move right
	if (Keyboard::Get()->Press('D'))
	{
		handleInput(PRESS_D);
	}

	//Player move left
	if (Keyboard::Get()->Press('A'))
	{
		handleInput(PRESS_A);
	}

	//Player rotation left
	if (Keyboard::Get()->Press('Q'))
	{
		playerRotationAngle(-1.0f);
	}

	//Player rotation right
	if (Keyboard::Get()->Press('E'))
	{
		playerRotationAngle(+1.0f);
	}

	if (Keyboard::Get()->Up('W') || Keyboard::Get()->Up('S') || Keyboard::Get()->Up('A') || Keyboard::Get()->Up('D'))
	{
		handleInput(RELEASE_MOVE);
	}

	//Player attack
	if (Keyboard::Get()->Down('F'))
	{
		handleInput(ATTACK);
	}
	handleInput(INPUT_NULL);
}

void Player::playerMovePos(Vector3 pos, bool plus)
{
	if (plus)
	{
		playerPos += pos;
	}
	else
	{
		playerPos -= pos;
	}
}

void Player::playerRotationAngle(float rotationAngle)
{
	angle += rotationAngle;
}

