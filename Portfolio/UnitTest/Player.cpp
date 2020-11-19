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
}

void Player::Render()
{
	//Insert Shader Pass
	for (ModelAnimator* temp : animators)	//TODO: pass
		temp->Pass(2);

	model->Render();
}


void Player::CreateArcherModel()
{
	modelShader = new Shader(L"27_Animation.fxo");
	archer = new ModelAnimator(modelShader);

	archer->ReadMaterial(L"Kachujin/Mesh");
	archer->ReadMesh(L"Kachujin/Mesh");

	archer->ReadClip(L"Kachujin/Idle");
	archer->PlayClip(0, 0, 1.0f);

	Transform* transform = NULL;

	transform = archer->AddTransform();
	transform->Position(0.0f, 0.0f, 0.0f);
	transform->Scale(0.075f, 0.075f, 0.075f);

	archer->UpdateTransforms();

	animators.push_back(archer);
}
