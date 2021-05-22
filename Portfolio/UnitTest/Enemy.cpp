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

		isLive[i] = true;
		bAttack[i] = false;
		bDamaged[i] = false;

		hp[i] = 3;
	}
}

void Enemy::Update()
{
	if (isRender == false) return;

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		state[i]->Update(*this, i);
		if (hp[i] == 0)
		{
			bMove[i] = false;
			bRotate[i] = false;
			handleState(E_STATE_DYING, i);
		}
	}
	model->Update();
	model->UpdateTransforms();
	AttachCollider();
}

void Enemy::Render()
{
	model->Render();

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		collider[i]->Collider->Render(Color(1, 0, 0, 1));

		if (bAttack[i] == true)
		{
			attackCollider[i]->Collider->Render(Color(0, 1, 0, 1));
		}
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
		rotateToPlayer(i, target);
	}
}

bool Enemy::IsIntersect(ColliderObjectDesc* other, UINT instance)
{
	if (isRender == false) return false;

	if (bAttack[instance] == false) return false;

	if (attackCollider[instance]->Collider->IsIntersect(other->Collider))
	{
		bAttack[instance] = false;

		return true;
	}

	return false;
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

void Enemy::CreateCollider(UINT instance, ColliderObjectDesc*& collider)
{
	collider = new ColliderObjectDesc();
	collider->Init = new Transform();
	collider->Init->Position(position.x, position.y + 100.0f, position.z);
	collider->Init->Scale(100.0f, 170.0f, 100.0f);

	collider->Transform = new Transform();
	collider->Collider = new Collider(collider->Transform, collider->Init);
}

void Enemy::CreateAttackCollider(ColliderObjectDesc*& collider, UINT instance, Vector3 srt[3])
{
	collider = new ColliderObjectDesc();
	collider->Init = new Transform();

	Vector3 colliderScale = srt[0];
	Vector3 colliderRotationDegree = srt[1];
	Vector3 colliderPosition = srt[2];

	collider->Init->Scale(colliderScale);
	collider->Init->RotationDegree(colliderRotationDegree);
	collider->Init->Position(colliderPosition);

	collider->Transform = new Transform();
	collider->Collider = new Collider(collider->Transform, collider->Init);
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

void Enemy::AttachAttackCollider(ColliderObjectDesc*& collider, UINT instance, UINT attachCollider)
{
	if (collider != NULL)
	{
		Matrix attach = model->GetAttachTransform(instance, attachCollider);

		collider->Collider->GetTransform()->World(attach);
		collider->Collider->Update();
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
	
	float dis = 0.0f;
	static Vector3 thisPos, thisRot, targetPos;

	model->GetTransform(instance)->Position(&thisPos);
	model->GetTransform(instance)->RotationDegree(&thisRot);
	target->GetTransform(0)->Position(&targetPos);

	dis = Math::Distance(thisPos, targetPos);

	if (dis < 30.0f)
	{
		bRotate[instance] = true;
	}
	else
	{
		bRotate[instance] = false;
	}

	if (dis < 15.0f)
	{
		handleState(E_STATE_ATTACK, instance);
	}
	else 
	{
		if (bMove[instance])
			handleState(E_STATE_RUNNING, instance);
	}

	if (bMove[instance] == false) return;
	if (bRotate[instance] == false) return;

	Vector3 enemyDIrZ;
	enemyDIrZ = -model->GetTransform(instance)->Forward();

	Vector3 lookToPlayerDir;
	targetPos.y = 0.0f;
	thisPos.y = 0.0f;
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

void Enemy::GetHeight(Terrain* terrain, UINT instance)
{
	static Vector3 enemyPos;
	model->GetTransform(instance)->Position(&enemyPos);

	enemyPos.y = terrain->GetHeight(enemyPos);

	model->GetTransform(instance)->Position(enemyPos);
}

void Enemy::minusHP(UINT instance, UINT num)
{
	if (hp[instance] <= 0) return;

	hp[instance] -= num;
}

void Enemy::Damaged(UINT instance, bool bDamaged)
{
	this->bDamaged[instance] = bDamaged;
}
