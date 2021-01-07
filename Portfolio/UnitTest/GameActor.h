#pragma once

class GameActor
{
public:
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void Move() = 0;
};