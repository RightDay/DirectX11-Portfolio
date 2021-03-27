#pragma once

class GameActor
{
public:
	virtual ~GameActor() {}

	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void Move() = 0;

	virtual void Jump() = 0;
	virtual void Run() = 0;
	virtual void Attack() = 0;

	void minusHP(UINT num);

public:
	UINT hp;
};