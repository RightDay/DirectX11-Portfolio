#pragma once
#include "Framework.h"

struct ParticleData
{
	enum class BlendType
	{
		Opaque = 0, Additive, AlphaBlend, NonPremultiplied
	} Type = BlendType::Opaque;

	
	wstring TextureFile = L"";

	UINT MaxParticles = 100;

	float Duration = 1.0f;
	float DurationRandomness = 0;

	float EmitterVelocitySensitivity = 1;

	float MinHorizontalVelocity = 0;
	float MaxHorizontalVelocity = 0;

	float MinVerticalVelocity = 0;
	float MaxVerticalVelocity = 0;

	Vector3 Gravity = Vector3(0, 0, 0);

	float EndVelocity = 1;

	Color MinColor = Color(1, 1, 1, 1);
	Color MaxColor = Color(1, 1, 1, 1);

	float MinRotateSpeed = 0;
	float MaxRotateSpeed = 0;

	float MinStartSize = 100;
	float MaxStartSize = 100;

	float MinEndSize = 100;
	float MaxEndSize = 100;
};