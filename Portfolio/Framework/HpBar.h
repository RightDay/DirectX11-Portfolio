#pragma once
#include "Meshes/Mesh.h"

class HpBar : public Mesh
{
public:
	typedef VertexTextureNormalTangent MeshVertex;

public:
	HpBar(Shader* shader, float w = 0.5f, float h = 0.5f);
	~HpBar();

	void SetShader(Shader* shader);
	void Pass(UINT val) { pass = val; }

	void Update();
	void Render(UINT drawCount);

	void SetWidth(float w);

	Transform* AddTransform();
	Transform* GetTransform(UINT index) { return transforms[index]; }
	void UpdateTransforms();

private:
	void Create() override;

	float w;
	float h;

	float width = 0;

protected:
	Shader* shader;
	UINT pass = 0;

	PerFrame* perFrame = NULL;

	VertexBuffer* vertexBuffer = NULL;
	IndexBuffer* indexBuffer = NULL;

	MeshVertex* vertices;
	UINT* indices;

	UINT vertexCount, indexCount;

private:
	vector<Transform*> transforms;
	Matrix worlds[MAX_MESH_INSTANCE];

	VertexBuffer* instanceBuffer;
};