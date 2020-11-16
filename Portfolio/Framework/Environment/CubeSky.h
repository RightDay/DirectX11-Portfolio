#pragma once

class CubeSky
{
public:
	CubeSky(wstring file, Shader* shader = NULL);
	~CubeSky();

	void Update();
	void Render();

	void Pass(UINT val) { pass = val; }

private:
	UINT pass = 0;

	bool bCreateShader = false;
	Shader* shader;
	MeshRender* sphereRender;

	ID3D11ShaderResourceView* srv;
	ID3DX11EffectShaderResourceVariable* sSrv;
};