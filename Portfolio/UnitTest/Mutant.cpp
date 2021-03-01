#include "stdafx.h"
#include "Mutant.h"

Mutant::Mutant()
	: Enemy(model)
{
	CreateModel(3);
}

Mutant::Mutant(ModelAnimator* model)
	: Enemy(model)
{
	CreateModel(3);
}

Mutant::~Mutant()
{
}

void Mutant::Update()
{
	//Super::Update();
	for (int i = 0; i < 3; i++)
	{
		model->Update();
	}
	
}

void Mutant::Render()
{
	//Super::Render();
	model->Render();
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
	model->ReadMesh(L"Enemy/Mutant");
	model->ReadClip(L"Enemy/Idle");
	{
		Transform* transform = NULL;

		for (int i = 0; i < modelNum; i++)
		{
			transform = model->AddTransform();

			Vector3 randomVec3;
			randomVec3 = Math::RandomVec3(10.0f, 30.0f);
			randomVec3.y = 0;

			transform->Position(randomVec3);
			transform->Scale(0.1f, 0.1f, 0.1f);
			model->PlayClip(i, 0, 1.0f);
		}
	}
	model->UpdateTransforms();
}
