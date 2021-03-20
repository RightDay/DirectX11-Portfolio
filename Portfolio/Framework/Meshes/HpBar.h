#pragma once
#include "Meshes/Mesh.h"

class HpBar : public Mesh
{
public:
	HpBar(float w = 0.5f, float h = 0.5f);
	~HpBar();

private:
	void Create() override;

	float w;
	float h;

protected:
	Shader* shader;
	UINT pass = 0;

	PerFrame* perFrame = NULL;

	VertexBuffer* vertexBuffer = NULL;
	IndexBuffer* indexBuffer = NULL;

	MeshVertex* vertices;
	UINT* indices;

	UINT vertexCount, indexCount;
};