#pragma once
#include "Camera.h"

class Fixity : public Camera
{
public:
	Fixity();
	~Fixity();

	void Update() override;
	void SetView(Matrix& view) { matView = view; }

private:

};