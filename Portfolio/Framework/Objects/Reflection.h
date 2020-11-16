#pragma once

class Reflection
{
public:
	Reflection(Shader* shader, Transform* transform, UINT width = 0, UINT height = 0);
	~Reflection();

	void Update();
	
	void PreRender();
	void Render();

private:
	Shader* shader;
	UINT width, height;

	Transform* transform;
	Matrix matrix;

	class Fixity* camera;
	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ID3DX11EffectShaderResourceVariable* sReflection;
	ID3DX11EffectMatrixVariable* sMatrix;
};