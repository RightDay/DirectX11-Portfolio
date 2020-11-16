#pragma once
#include "ParticleData.h"

class ParticleSystem : public Renderer
{
public:
	ParticleSystem(wstring file);
	~ParticleSystem();

	void Update();
	void Render();

	void Add(Vector3& position);
	ParticleData& GetData() { return data; }

	void Reset();

	void SetTexture(wstring file);

private:
	void ReadFile(wstring file);

	void MapVertices();
	void Activate();
	void Deactivate();

private:
	struct VertexParticle
	{
		Vector3 Position;
		Vector2 Corner;
		Vector3 Velocity;
		Color Random;
		float Time;
	};

private:
	struct Desc
	{
		Color MinColor;
		Color MaxColor;

		Vector3 Gravity;
		float EndVelocity;

		Vector2 StartSize;
		Vector2 EndSize;

		Vector2 RotateSpeed;
		float Duration;
		float DurationRandomness;

		float CurrentTime;
		float Padding[3];
	} desc;

private:
	wstring file;

	ParticleData data;

	float currentTime = 0.0f;
	float lastAddTime = 0.0f;

	VertexParticle* vertices = NULL;
	UINT* indices = NULL;

	UINT activeCount = 0;
	UINT newCount = 0;
	UINT freeCount = 0;
	UINT deactiveCount = 0;


	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	Texture* map = NULL;
	ID3DX11EffectShaderResourceVariable* sMap;
};