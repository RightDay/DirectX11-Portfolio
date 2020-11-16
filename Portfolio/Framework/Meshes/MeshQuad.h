#pragma once
#include "Mesh.h"

class MeshQuad : public Mesh
{
public:
	MeshQuad();
	~MeshQuad();

private:
	void Create() override;
};