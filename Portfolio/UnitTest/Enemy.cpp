#include "stdafx.h"
#include "Enemy.h"
#include "EnemyState.h"

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
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		state[i] = new E_MovingState();
		CreateCollider(i, collider[i]);
	}
}

void Enemy::Update()
{
	model->Update();
	model->UpdateTransforms();
	AttachCollider();
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		state[i]->Update(*this, i);
	}
}

void Enemy::Render()
{
	model->Render();
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		collider[i]->Collider->Render(Color(0, 0, 1, 1));
	}

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
		//moveForward(i);
		//rotateAccordingToDistance(i, 50.0f);
		rotateToPlayer(i, target);
	}
}

void Enemy::CreateModel()
{
}

void Enemy::handleState(eAnimState returnState, UINT instance)
{
	EnemyState* state_ = state[instance]->handleState(returnState);
	if (state_ != NULL)
	{
		delete state[instance];
		state[instance] = state_;

		state[instance]->Enter(*this, instance);
	}
}

void Enemy::CreateCollider(UINT instance, ColliderObjectDesc* collider)
{
	this->collider[instance] = new ColliderObjectDesc();
	this->collider[instance]->Init = new Transform();
	this->collider[instance]->Init->Position(position.x, position.y + 100.0f, position.z);
	this->collider[instance]->Init->Scale(60.0f, 170.0f, 70.0f);

	this->collider[instance]->Transform = new Transform();
	this->collider[instance]->Collider = new Collider(this->collider[instance]->Transform, this->collider[instance]->Init);
	//collider = new ColliderObjectDesc();
	//collider->Init = new Transform();
	//collider->Init->Position(position.x, position.y, position.z);
	//collider->Init->Scale(60.0f, 170.0f, 70.0f);

	//collider->Transform = new Transform();
	//collider->Collider = new Collider(collider->Transform, collider->Init);
}

void Enemy::AttachCollider()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		Matrix attach = model->GetTransform(i)->World();
		collider[i]->Collider->GetTransform()->World(attach);

		collider[i]->Collider->Update();
	}
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
	static bool isRotate = true;
	float dis = 0.0f;
	static Vector3 thisPos, thisRot, targetPos;

	model->GetTransform(instance)->Position(&thisPos);
	model->GetTransform(instance)->RotationDegree(&thisRot);
	target->GetTransform(0)->Position(&targetPos);

	dis = Math::Distance(thisPos, targetPos);
	ImGui::Text("Distance : %f", dis);
	ImGui::Text("isRotate : %d", isRotate);

	if (dis < 30.0f)
	{
		isRotate = false;
	}

	else
	{
		isRotate = true;
	}

	if (dis < 10.0f)
	{
		handleState(E_STATE_ATTACK, instance);
	}
	else
	{
		handleState(E_STATE_RUNNING, instance);
	}

	if (isRotate == true) return;

	Vector3 enemyDIrZ;
	enemyDIrZ = -model->GetTransform(instance)->Forward();

	Vector3 lookToPlayerDir;
	lookToPlayerDir = targetPos - thisPos;

	Vector3 enemyDirX;
	enemyDirX = -model->GetTransform(instance)->Right();

	D3DXVec3Normalize(&enemyDIrZ, &enemyDIrZ);
	D3DXVec3Normalize(&lookToPlayerDir, &lookToPlayerDir);
	D3DXVec3Normalize(&enemyDirX, &enemyDirX);

	float angleEnemyZAndPlayer = D3DXVec3Dot(&enemyDIrZ, &lookToPlayerDir);
	float angleEnemyXAndPlayer = D3DXVec3Dot(&enemyDirX, &lookToPlayerDir);

	angleEnemyXAndPlayer = acosf(angleEnemyXAndPlayer);
	angleEnemyXAndPlayer = Math::ToDegree(angleEnemyXAndPlayer);

	if (angleEnemyXAndPlayer > 90.0f)
	{
		angleEnemyZAndPlayer = -acosf(angleEnemyZAndPlayer);
		angleEnemyZAndPlayer = Math::ToDegree(angleEnemyZAndPlayer);
	}
	else if (angleEnemyXAndPlayer <= 90.0f)
	{
		angleEnemyZAndPlayer = acosf(angleEnemyZAndPlayer);
		angleEnemyZAndPlayer = Math::ToDegree(angleEnemyZAndPlayer);
	}
	if (isnan(angleEnemyZAndPlayer) == false)
	{
		thisRot.y += angleEnemyZAndPlayer;
	}
	model->GetTransform(instance)->RotationDegree(thisRot);
}
