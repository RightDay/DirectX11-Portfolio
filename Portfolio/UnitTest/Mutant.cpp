#include "stdafx.h"
#include "Mutant.h"

Mutant::Mutant()
{
	CreateModel(ENEMY_NUM);

	Initialize(model);
}

Mutant::Mutant(ModelAnimator* model)
	: Enemy(model)
{
	CreateModel(ENEMY_NUM);
}

Mutant::~Mutant()
{
}

void Mutant::Update()
{
	Super::Update();
}

void Mutant::Render()
{
	Super::Render();
}

void Mutant::Move()
{
}

void Mutant::Jump()
{
}

void Mutant::Run()
{
}

void Mutant::Attack()
{
}

void Mutant::CreateModel(UINT modelNum)
{
	modelShader = new Shader(L"27_Animation.fxo");
	model = new ModelAnimator(modelShader);

	model->ReadMaterial(L"Enemy/Mutant");
	model->ReadMesh(L"Enemy/Mutant/Mutant");
	model->ReadClip(L"Enemy/Mutant/Mutant_Idle");
	model->ReadClip(L"Enemy/Mutant/Mutant_Walking");
	model->ReadClip(L"Enemy/Mutant/Mutant_Run");
	model->ReadClip(L"Enemy/Mutant/Mutant_Attack");
	{
		Transform* transform = NULL;

		for (int i = 0; i < modelNum; i++)
		{
			transform = model->AddTransform();

			Vector3 randomVec3;
			randomVec3 = Math::RandomVec3(-30.0f, 30.0f);
			randomVec3.y = 0;

			transform->Position(randomVec3);
			transform->Scale(0.1f, 0.1f, 0.1f);
			model->PlayClip(i, M_WALK, 1.0f);
		}
	}
	model->UpdateTransforms();
}