#pragma once
#define MAX_POSTEFFECT_SRVS 8

class PostEffect
{
public:
	PostEffect(wstring shaderFile, ID3D11ShaderResourceView** gBufferSrvs = NULL);
	virtual ~PostEffect();

	virtual void Update();
	virtual void PreRender(DepthStencil* depthStencil);
	virtual void Render();

	void Srv(UINT index, ID3D11ShaderResourceView* srv);

	Shader* GetShader() { return shader; }
	
	void Technique(UINT val) { technique = val; }
	void Pass(UINT val) { pass = val; }

	ID3D11ShaderResourceView* RenderTargetSRV(UINT index)
	{
		return renderTarget[index]->SRV();
	}

private:
	struct Desc
	{
		Vector2 PixelSize;
		float Padding[2];
	} desc;

public:
	struct ValueDesc
	{
		float Saturation = 4.0f;
		float Sharpness = 3.0f;
		float Padding[2];
		
		Vector2 VignetteScale = Vector2(2, 2);
		float VignettePower = 2.0f;
		
		float InteraceStrength = 1.0f;
		int InteraceValue = 2;
		float Padding2[3];

		Vector3 Distortion = Vector3(0, 0, 0);
		float LensPower = 1.0f;

		float Laplacian = 1.0f;

		float Padding3[3];
	};

	static ValueDesc& GetValues() { return valueDesc; }
private:
	static ValueDesc valueDesc;

protected:
	UINT technique = 0;
	UINT pass = 0;
	Shader* shader;

	RenderTarget* renderTarget[4];

private:
	wstring shaderFile;

	Transform* transform;
	PerFrame* perFrame;

	VertexBuffer* vertexBuffer;
	
	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	ConstantBuffer* valueBuffer;
	ID3DX11EffectConstantBuffer* sValueBuffer;


	ID3D11ShaderResourceView* srvs[MAX_POSTEFFECT_SRVS];
	ID3DX11EffectShaderResourceVariable* sSrvs;

	ID3D11ShaderResourceView** gBufferSrvs;
	ID3DX11EffectShaderResourceVariable* sGBufferSrvs;
};