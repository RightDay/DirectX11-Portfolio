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
	playerForward = model->GetTransform(0)->Forward();
	playerRight = model->GetTransform(0)->Right();

	moveVertical = playerForward * velocity * Time::Delta();
	moveHorizontal = playerRight * velocity * Time::Delta();

	if (isMove == true) return;

	//Player move forward
	if (Keyboard::Get()->Press('W'))
	{
		playerPos += -moveVertical;
	}

	//Player move backward
	if (Keyboard::Get()->Press('S'))
	{
		playerPos += moveVertical;
	}

	//Player move right
	if (Keyboard::Get()->Press('D'))
	{
		playerPos += -moveHorizontal;
	}

	//Player move left
	if (Keyboard::Get()->Press('A'))
	{
		playerPos += moveHorizontal;
	}

	model->GetTransform(0)->Position(playerPos);
	model->UpdateTransforms();

}

void Player::Rotation()
{
	//Player move right
	if (Keyboard::Get()->Press('Q'))
	{
		angle -= 0.5f;
	}

	//Player move left
	if (Keyboard::Get()->Press('E'))
	{
		angle += 0.5f;
	}

	playerRot = Vector3(0.0f, angle, 0.0f);

	model->GetTransform(0)->RotationDegree(playerRot);	
}
