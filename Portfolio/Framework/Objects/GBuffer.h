#pragma once

class GBuffer
{
public:
	GBuffer(Shader* shader, UINT width = 0, UINT height = 0);
	~GBuffer();

	void PreRender();
	void Render(RenderTarget* rtv = NULL);
	void PostRender();

	void DrawDebug(bool val) { bDrawDebug = val; }

	ID3D11ShaderResourceView** Srvs() { return srvs; }

private:
	void DirectionalLight();
	
	void CalcPointLights(UINT count);
	void PointLights();

	void CalcSpotLights(UINT count);
	void SpotLights();

	void CreateRasterizerState();
	void CreateDepthStencilView();
	void CreateDepthStencilState();

private:
	struct PointLightDesc
	{
		float TessFactor = 8.0f;
		float Padding[3];

		Matrix Projection[MAX_POINT_LIGHT];
		PointLight PointLight[MAX_POINT_LIGHT];
	} pointLightDesc;

	struct SpotLightDesc
	{
		float TessFactor = 8.0f;
		float Padding[3];

		Vector4 Angle[MAX_POINT_LIGHT];
		Matrix Projection[MAX_POINT_LIGHT];

		SpotLight SpotLight[MAX_POINT_LIGHT];
	} spotLightDesc;

private:
	bool bDrawDebug = false;

	Shader* shader;
	UINT width, height;

	RenderTarget* diffuseRTV; //Color(24), Alpha(8) - R8G8B8A8
	RenderTarget* specularRTV; //Color(24), Power(8) - R8G8B8A8
	RenderTarget* emissiveRTV; //Color(24), Power(8) - R8G8B8A8
	RenderTarget* normalRTV; //X(32), Y(32), Z(32) - R32B32G32A32
	RenderTarget* tangentRTV; //X(32), Y(32), Z(32) - R32B32G32A32
	DepthStencil* depthStencil;
	Viewport* viewport;

	ID3D11ShaderResourceView* srvs[6];

	Render2D* render2D[5];

	ConstantBuffer* pointLightBuffer;
	ID3DX11EffectConstantBuffer* sPointLightBuffer;

	ConstantBuffer* spotLightBuffer;
	ID3DX11EffectConstantBuffer* sSpotLightBuffer;

	ID3DX11EffectShaderResourceVariable* sSrvs;

	ID3D11RasterizerState* debugRSS;
	ID3D11RasterizerState* lightRSS;
	ID3DX11EffectRasterizerVariable* sRSS;

	ID3D11DepthStencilView* depthStencilReadOnly;
	
	ID3D11DepthStencilState* packDSS;
	ID3D11DepthStencilState* directionalLightDSS;
	ID3D11DepthStencilState* lightDSS;
	ID3DX11EffectDepthStencilVariable* sDSS;
};