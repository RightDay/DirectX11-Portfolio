#include "stdafx.h"
#include "Mutant.h"

Mutant::Mutant()
{
	CreateModel(ENEMY_NUM);

	Initialize(model);

	lefthandBoneNumber = 12;

	attackColliderSRT[0] = Vector3(100.0f, 40.0f, 40.0f);
	attackColliderSRT[1] = Vector3(0.0f, 0.0f, 0.0f);
	attackColliderSRT[2] = Vector3(25.0, 0.0f, 0.0f);

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		CreateAttackCollider(attackCollider[i], i, attackColliderSRT);
	}
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

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		ImGui::Text("Mutant[%d] hp : %d", i, hp[i]);

		if (bAttack[i] == true)
			AttachAttackCollider(attackCollider[i], i, lefthandBoneNumber);
	}
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
	model->ReadClip(L"Enemy/Mutant/Mutant_Run");
	model->ReadClip(L"Enemy/Mutant/Mutant_Attack");
	model->ReadClip(L"Enemy/Mutant/Mutant_Dying");
	model->ReadClip(L"Enemy/Mutant/Mutant_Idle");
	model->ReadClip(L"Enemy/Mutant/Mutant_Walking");
	{
		Transform* transform = NULL;

		for (int i = 0; i < modelNum; i++)
		{
			transform = model->AddTransform();

			Vector3 randomVec3;
			randomVec3 = Math::RandomVec3(0.0f, 512.0f);
			randomVec3.y = 0.0f;

			transform->Position(randomVec3);
			transform->Scale(0.1f, 0.1f, 0.1f);
			//model->PlayClip(i, M_WALK, 1.0f);
		}
	}
	model->UpdateTransforms();
}