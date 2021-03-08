#pragma once

class EnemyState
{
public:
	virtual ~EnemyState() {}
	//virtual void Enter(Enemy& enemy, UINT instance) {}
	virtual void Enter(ModelAnimator* model, UINT instance) {}
	virtual EnemyState* handleState(Enemy::eAnimState animState) { return this; }
	virtual void Update(Enemy& enemy) {}
};

class MovingState : public EnemyState
{
public:
	MovingState() {}
	//void Enter(Enemy& enemy, UINT instance) override;
	void Enter(ModelAnimator* model, UINT instance) override;
	EnemyState* handleState(Enemy::eAnimState animState) override;
	void Update(Enemy& Enemy) override;
};

class AttackState : public EnemyState
{
public:
	AttackState() {}
	//void Enter(Enemy& enemy, UINT instance) override;
	void Enter(ModelAnimator* model, UINT instance) override;
	EnemyState* handleState(Enemy::eAnimState animState) override;
	void Update(Enemy& Enemy) override;
};