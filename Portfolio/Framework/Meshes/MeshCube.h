#pragma once
#include "Mesh.h"

class MeshCube : public Mesh
{
public:
	MeshCube();
	~MeshCube();

private:
	void Create() override;
};