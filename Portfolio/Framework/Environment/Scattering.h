#pragma once

class Scattering
{
public:
	Scattering(Shader* shader);
	~Scattering();

	void Update();

	void PreRender();
	void PostRender();

	void Pass(UINT val) { pass = val; }
	void SetDebug(bool val) { bDebug = val; }

	RenderTarget* RayleighRTV() { return rayleighTarget; }
	RenderTarget* MieRTV() { return mieTarget; }

private:
	void CreateQuad();

private:
	bool bDebug = false;
	UINT pass = 0;

	UINT width, height;

	Shader* shader;
	Render2D* render2D;

	RenderTarget* mieTarget, *rayleighTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	VertexBuffer* vertexBuffer;
};