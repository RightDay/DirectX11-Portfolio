#pragma once

class Moon : public Renderer
{
public:
	Moon(Shader* shader);
	~Moon();

	void Update();
	void Render(float theta);

private:
	float GetAlpha(float theta);

	Matrix GetTransform(float theta);
	Matrix GetGlowTransform(float theta);

private:
	float distance, glowDistance;

	ID3DX11EffectScalarVariable* sAlpha;

	Texture* moon;
	Texture* moonGlow;
	ID3DX11EffectShaderResourceVariable* sMoon;
};