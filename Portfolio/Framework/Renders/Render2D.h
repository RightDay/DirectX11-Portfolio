#pragma once

class Render2D
{
public:
	Render2D(wstring shaderFile = L"");
	virtual ~Render2D();

	virtual void Update();
	virtual void Render();

	void Pass(UINT val) { pass = val; }
	Shader* GetShader() { return shader; }


	Transform* GetTransform() { return transform; }
	virtual void SRV(ID3D11ShaderResourceView* srv);

private:
	struct ViewProjectionDesc
	{
		Matrix View;
		Matrix Projection;
	} vpDesc;

protected:
	UINT pass;
	Shader* shader;

	Vector2 textureSize;

private:
	Transform* transform;
	PerFrame* perFrame;

	ConstantBuffer* vpBuffer;
	ID3DX11EffectConstantBuffer* sVpBuffer;

	VertexBuffer* vertexBuffer;

	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
};