#pragma once

class EnemyState
{
public:
	virtual ~EnemyState() {}
	virtual void Enter(Enemy& enemy, UINT instance) {}
	//virtual void Enter(ModelAnimator* model, UINT instance) {}
	virtual EnemyState* handleState(Enemy::eAnimState animState) { return this; }
	virtual void Update(Enemy& enemy, UINT instance) {}
};

class E_MovingState : public EnemyState
{
public:
	E_MovingState() {}
	void Enter(Enemy& enemy, UINT instance) override;
	//void Enter(ModelAnimator* model, UINT instance) override;
	EnemyState* handleState(Enemy::eAnimState animState) override;
	void Update(Enemy& enemy, UINT instance) override;
};

class E_AttackState : public EnemyState
{
public:
	E_AttackState() {}
	void Enter(Enemy& enemy, UINT instance) override;
	EnemyState* handleState(Enemy::eAnimState animState) override;
	void Update(Enemy& enemy, UINT instance) override;
};