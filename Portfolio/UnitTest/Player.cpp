#include "stdafx.h"
#include "Player.h"

Player::Player()
{
	CreateArcherModel();

	model = archer;
}

Player::Player(ModelAnimator* model)
	:model(model)
{
	
}

Player::~Player()
{
}

void Player::Update()
{
	model->Update();

	Move();
	Rotation();
}

void Player::Render()
{
	//Insert Shader Pass

	model->Render();
}


void Player::CreateArcherModel()
{
	modelShader = new Shader(L"27_Animation.fxo");
	archer = new ModelAnimator(modelShader);

	archer->ReadMaterial(L"Archer/Archer");
	archer->ReadMesh(L"Archer/Archer");

	archer->ReadClip(L"Archer/Idle");
	archer->ReadClip(L"Archer/Hip_Hop_Dancing");
	archer->ReadClip(L"Archer/Jump");
	archer->ReadClip(L"Archer/Running");

	archer->PlayClip(0, STATE_IDLE, 1.0f);

	Transform* transform = NULL;

	transform = archer->AddTransform();
	transform->Position(0.0f, 0.0f, 0.0f);
	transform->RotationDegree(0.0f, 180.0f, 0.0f);
	transform->Scale(0.075f, 0.075f, 0.075f);

	archer->UpdateTransforms();

	animators.push_back(archer);

	for (ModelAnimator* temp : animators)
		temp->Pass(2);
}

void Player::Move()
{
	if (Mouse::Get()->Press(1) == true) return;

	playerForward = model->GetTransform(0)->Forward();
	playerRight = model->GetTransform(0)->Right();
	
	moveVertical = playerForward * velocity * Time::Delta();
	moveHorizontal = playerRight * velocity * Time::Delta();

	if (isMove == false) return;

	playerControl();

	model->GetTransform(0)->Position(playerPos);
	model->UpdateTransforms();
}

void Player::Rotation()
{
	playerControl();

	playerRot = Vector3(0.0f, angle, 0.0f);

	model->GetTransform(0)->RotationDegree(playerRot);
}

void Player::playerControl()
{
	//Player move forward
	if (Keyboard::Get()->Press('W'))
	{
		playerMovePos(moveVertical, false);
	}

	//Player move backward
	if (Keyboard::Get()->Press('S'))
	{
		playerMovePos(moveVertical, true);
	}

	//Player move right
	if (Keyboard::Get()->Press('D'))
	{
		playerMovePos(moveHorizontal, false);
	}

	//Player move left
	if (Keyboard::Get()->Press('A'))
	{
		playerMovePos(moveHorizontal, true);
	}

	//Player rotation left
	if (Keyboard::Get()->Press('Q'))
	{
		playerRotationAngle(-0.5f);
	}

	//Player rotation right
	if (Keyboard::Get()->Press('E'))
	{
		playerRotationAngle(+0.5f);
	}
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

