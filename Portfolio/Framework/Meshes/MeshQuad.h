#pragma once
#include "Mesh.h"

class MeshQuad : public Mesh
{
public:
	MeshQuad(float w = 0.5f, float h = 0.5f);
	~MeshQuad();

private:
	void Create() override;

	float w;
	float h;
};