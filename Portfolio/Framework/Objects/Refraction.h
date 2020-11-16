#pragma once

class Refraction
{
public:
	Refraction(Shader* shader, wstring normalFile, UINT width = 0, UINT height = 0);
	~Refraction();

	void Update();
	
	void PreRender();
	void Render();

private:
	Shader* shader;
	UINT width, height;

	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ID3DX11EffectShaderResourceVariable* sRefraction;


	Texture* normalMap;
	ID3DX11EffectShaderResourceVariable* sNormalMap;
};