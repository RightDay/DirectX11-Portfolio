#pragma once

class ModelRender
{
public:
	ModelRender(Shader* shader);
	~ModelRender();

	void Update();
	void Render();

public:
	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);

	Model* GetModel() { return model; }

	void Pass(UINT pass);

	Transform* AddTransform();
	Transform* GetTransform(UINT index) { return transforms[index]; }
	void UpdateTransforms();

	void UpdateTransform(UINT instanceId, UINT boneIndex, Transform& transform);

private:
	void CreateTexture();

private:
	Shader* shader;
	Model* model;

	Matrix bones[MAX_MODEL_TRANSFORMS];
	Matrix boneTransforms[MAX_MODEL_INSTANCE][MAX_MODEL_TRANSFORMS];

	vector<Transform *> transforms;
	Matrix worlds[MAX_MODEL_INSTANCE];

	VertexBuffer* instanceBuffer;

	ID3D11Texture2D* texture = NULL;
	ID3D11ShaderResourceView* srv;
};