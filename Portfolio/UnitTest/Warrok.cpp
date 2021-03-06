#include "stdafx.h"
#include "Warrok.h"
#include "Player.h"

Warrok::Warrok()
{
	CreateModel(ENEMY_NUM);

	Initialize(model);
}

Warrok::Warrok(ModelAnimator* model)
	: Enemy(model)
{
	CreateModel(ENEMY_NUM);
}

Warrok::~Warrok()
{
}

void Warrok::Update()
{
	Super::Update();
}

void Warrok::Render()
{
	Super::Render();
}

void Warrok::Move()
{
}

void Warrok::Jump()
{
}

void Warrok::Run()
{
}

void Warrok::Attack()
{
}

void Warrok::CreateModel(UINT modelNum)
{
	modelShader = new Shader(L"27_Animation.fxo");
	model = new ModelAnimator(modelShader);

	model->ReadMaterial(L"Enemy/Warrok");
	model->ReadMesh(L"Enemy/Warrok/Warrok");
	model->ReadClip(L"Enemy/Warrok/Warrok_Walk");
	model->ReadClip(L"Enemy/Warrok/Warrok_Attack");
	{
		Transform* transform = NULL;

		for (int i = 0; i < modelNum; i++)
		{
			transform = model->AddTransform();

			Vector3 randomVec3;
			randomVec3 = Math::RandomVec3(-30.0f, 30.0f);
			randomVec3.y = 2.0f;

			transform->Position(randomVec3);
			transform->Scale(0.1f, 0.1f, 0.1f);
			model->PlayClip(i, W_WALK, 1.0f);
		}
	}
	model->UpdateTransforms();
}
