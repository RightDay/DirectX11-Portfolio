#include "stdafx.h"
#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::Enemy(ModelAnimator* model)
	:model(model)
{
}

Enemy::~Enemy()
{
}

void Enemy::Initialize(ModelAnimator* model)
{
	this->model = model;
}

void Enemy::Update()
{
	model->Update();
	model->UpdateTransforms();
}

void Enemy::Render()
{
	model->Render();

	if (isRender == false)
		isRender = true;
}

void Enemy::Move()
{
}

void Enemy::Jump()
{
}

void Enemy::Run()
{
}

void Enemy::Attack()
{
}

void Enemy::Patrol(ModelAnimator* target)
{
	static Vector3 enemyPos = Vector3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		moveForward(i);
		rotateAccordingToDistance(i, 50.0f);
		rotateToPlayer(i, target);
	}
}

void Enemy::CreateModel()
{
}

void Enemy::moveForward(UINT instance)
{
	forward = model->GetTransform(instance)->Forward();

	if (isRender)
	{
		moveVertical = forward * velocity * Time::Delta() * 2.0f;
	}

	model->GetTransform(instance)->Position(&position);
	position -= moveVertical;

	model->GetTransform(instance)->Position(position);
}

void Enemy::rotateAccordingToDistance(UINT instance, float distance)
{
	static float enemyMovePos[ENEMY_NUM];

	static float randomAngle = 0.0f;

	enemyMovePos[instance] += Math::Length(moveVertical.x, moveVertical.z);

	if (enemyMovePos[instance] > distance)
	{
		randomAngle = Math::Random(-180.0f, 180.0f);

		model->GetTransform(instance)->RotationDegree(0.0f, randomAngle, 0.0f);

		enemyMovePos[instance] = 0.0f;
	}
}

void Enemy::rotateToPlayer(int instance, ModelAnimator* target)
{
	static bool isRotate = false;
	float dis = 0.0f;
	//arrival = playerPos;
	Vector3 thisRot, thisPos, targetPos;

	model->GetTransform(instance)->RotationDegree(&thisRot);
	model->GetTransform(instance)->Position(&thisPos);
	target->GetTransform(0)->Position(&targetPos);

	dis = Math::Distance(thisPos, targetPos);

	if (dis < 30.0f)
	{
		isRotate = false;
	}
	else
	{
		isRotate = true;
	}

	//if (isRotate == true) return;

	Vector3 enemyDIrZ;
	enemyDIrZ = -model->GetTransform(instance)->Forward();
	ImGui::Text("enemyDIrZ : %f. %f, %f", enemyDIrZ.x, enemyDIrZ.y, enemyDIrZ.z);

	Vector3 lookToPlayerDir;
	lookToPlayerDir = targetPos - thisPos;
	ImGui::Text("lookToPlayerDir : %f. %f, %f", lookToPlayerDir.x, lookToPlayerDir.y, lookToPlayerDir.z);

	Vector3 enemyDirX;
	enemyDirX = -model->GetTransform(instance)->Right();
	ImGui::Text("enemyDirX : %f. %f, %f", enemyDirX.x, enemyDirX.y, enemyDirX.z);

	D3DXVec3Normalize(&enemyDIrZ, &enemyDIrZ);
	D3DXVec3Normalize(&lookToPlayerDir, &lookToPlayerDir);
	D3DXVec3Normalize(&enemyDirX, &enemyDirX);

	float angleEnemyZAndPlayer = D3DXVec3Dot(&enemyDIrZ, &lookToPlayerDir);
	float angleEnemyXAndPlayer = D3DXVec3Dot(&enemyDirX, &lookToPlayerDir);
	ImGui::Text("angleEnemyXAndPlayer(D3DXVec3Dot) : %f", angleEnemyXAndPlayer);

	angleEnemyXAndPlayer = acosf(angleEnemyXAndPlayer);
	ImGui::Text("angleEnemyXAndPlayer(acos) : %f", angleEnemyXAndPlayer);
	angleEnemyXAndPlayer = Math::ToDegree(angleEnemyXAndPlayer);
	ImGui::Text("angleEnemyXAndPlayer : %f", angleEnemyXAndPlayer);

	if (angleEnemyXAndPlayer >= 90.0f)
	{
		angleEnemyZAndPlayer = -acosf(angleEnemyZAndPlayer);
		ImGui::Text("angleEnemyZAndPlayer > 90.0f : %f", angleEnemyZAndPlayer);

		angleEnemyZAndPlayer = Math::ToDegree(angleEnemyZAndPlayer);
		ImGui::Text("angleEnemyZAndPlayer > 90.0f : %f", angleEnemyZAndPlayer);
	}
	else if (angleEnemyXAndPlayer < 90.0f)
	{
		angleEnemyZAndPlayer = acosf(angleEnemyZAndPlayer);
		angleEnemyZAndPlayer = Math::ToDegree(angleEnemyZAndPlayer);
		ImGui::Text("angleEnemyZAndPlayer < 90.0f : %f", angleEnemyZAndPlayer);
	}
	if (isnan(angleEnemyZAndPlayer) == false)
	{
		thisRot.y += angleEnemyZAndPlayer;
	}

	model->GetTransform(instance)->RotationDegree(thisRot);
}
