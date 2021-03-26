#pragma once

class PlayerState 
{
public:
	virtual ~PlayerState() {}
	virtual void Enter(Player& player) {}
	virtual PlayerState* handleInput(Player& player, Input input) { return this; }
	virtual void Update(Player& player) {}
};

class StandingState : public PlayerState
{
public:
	StandingState() {}
	void Enter(Player& player) override;
	PlayerState* handleInput(Player& player, Input input) override;
	void Update(Player& player) override;
};

class MovingState : public PlayerState
{
public:
	MovingState() {}
	MovingState(Player player, Input input);
	void Enter(Player& player) override;
	PlayerState* handleInput(Player& player, Input input) override;
	void Update(Player& player) override;

private:
	void CalcurateDirection();
	void Rotate(Player player);
	void Move(Player player);

	Vector2 inputDir = Vector2(0, 0);
	float angle = 0.0f;
	bool bMove = false;
};

class AttackState : public PlayerState
{
public:
	AttackState() {}
	void Enter(Player& player) override;
	PlayerState* handleInput(Player& player, Input input) override;
	void Update(Player& player) override;

	PlayerState* isStop();

private:
	bool attackStop = false;
};