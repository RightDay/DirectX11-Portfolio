#pragma once
#include "Enemy.h"

class Warrok : public Enemy
{
public:
	Warrok();
	Warrok(ModelAnimator* model);
	~Warrok();

	void Update() override;
	void Render() override;

	void Move() override;
	void Jump() override;
	void Run() override;
	void Attack() override;

public:
	ModelAnimator* GetModel() { return model; }

private:
	void CreateModel(UINT modelNum);

private:
	ModelAnimator* model = NULL;
	Shader* modelShader = NULL;
};