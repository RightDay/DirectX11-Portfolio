#include "Framework.h"
#include "Lights.h"

Lights* Lights::instance = NULL;

Lights * Lights::Get()
{
	assert(instance != NULL);

	return instance;
}

void Lights::Create()
{
	assert(instance == NULL);

	instance = new Lights();
}

void Lights::Delete()
{
	SafeDelete(instance);
}

Lights::Lights()
{
}

Lights::~Lights()
{
}

UINT Lights::PointLights(OUT PointLight * lights)
{
	memcpy(lights, pointLights, sizeof(PointLight) * pointLightCount);

	return pointLightCount;
}

void Lights::AddPointLight(PointLight & light)
{
	pointLights[pointLightCount] = light;
	pointLightCount++;
}

PointLight & Lights::GetPointLight(UINT index)
{
	return pointLights[index];
}

UINT Lights::SpotLights(OUT SpotLight * lights)
{
	memcpy(lights, spotLights, sizeof(SpotLight) * spotLightCount);

	return spotLightCount;
}

void Lights::AddSpotLight(SpotLight & light)
{
	spotLights[spotLightCount] = light;
	spotLightCount++;
}

SpotLight & Lights::GetSpotLight(UINT index)
{
	return spotLights[index];
}