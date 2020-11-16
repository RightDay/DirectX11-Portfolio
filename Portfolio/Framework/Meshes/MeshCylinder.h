#pragma once
#include "Mesh.h"

class MeshCylinder : public Mesh
{
public:
	MeshCylinder(float radius, float height, UINT sliceCount = 10, UINT stackCount = 10);
	~MeshCylinder();

protected:
	void Create() override;

private:
	void BuildTopCap(vector<MeshVertex>& vertices, vector<UINT>& indices);
	void BuildBottomCap(vector<MeshVertex>& vertices, vector<UINT>& indices);

private:
	float topRadius;
	float bottomRadius;
	float height;

	UINT sliceCount;
	UINT stackCount;
};