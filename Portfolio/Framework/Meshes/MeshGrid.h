#pragma once

#include "Mesh.h"

class MeshGrid : public Mesh
{
public:
	MeshGrid(float offsetU = 1.0f, float offsetV = 1.0f);
	~MeshGrid();

protected:
	void Create() override;

	float offsetU;
	float offsetV;
};