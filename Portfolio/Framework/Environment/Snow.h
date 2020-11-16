#pragma once

class Snow : public Renderer
{
public:
	Snow(Vector3& extent, UINT count, wstring file);
	~Snow();

	void Update();
	void Render();

private:
	struct Desc
	{
		D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1);

		Vector3 Velocity = Vector3(0, -10, 0);
		float DrawDistance = 0;

		Vector3 Origin = Vector3(0, 0, 0);
		float Turbulence = 20;

		Vector3 Extent = Vector3(0, 0, 0);
		float Padding2;
	} desc;

private:
	struct VertexSnow
	{
		Vector3 Position;
		Vector2 Uv;
		float Scale;
		Vector2 Random;
	};

private:
private:
	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	VertexSnow* vertices;
	UINT* indices;

	Texture* texture;
	UINT drawCount;
};