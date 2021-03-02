#include "stdafx.h"
#include "Warrok.h"

Warrok::Warrok()
	: Enemy(model)
{
	CreateModel(3);
}

Warrok::Warrok(ModelAnimator* model)
	: Enemy(model)
{
	CreateModel(3);
}

Warrok::~Warrok()
{
}

void Warrok::Update()
{
	//Super::Update();
	for (int i = 0; i < 3; i++)
	{
		model->Update();
	}

}

void Warrok::Render()
{
	//Super::Render();
	model->Render();
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
	model->ReadMesh(L"Enemy/Warrok");
	model->ReadClip(L"Enemy/Walk");
	{
		Transform* transform = NULL;

		for (int i = 0; i < modelNum; i++)
		{
			transform = model->AddTransform();

			Vector3 randomVec3;
			randomVec3 = Math::RandomVec3(0.0f, 30.0f);
			randomVec3.y = 0.0f;

			transform->Position(randomVec3);
			transform->Scale(0.1f, 0.1f, 0.1f);
			model->PlayClip(i, 0, 0.5f);
		}
	}
	model->UpdateTransforms();
}
