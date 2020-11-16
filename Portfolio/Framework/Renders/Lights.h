#pragma once

#define MAX_POINT_LIGHT 256
#define MAX_SPOT_LIGHT 256

struct PointLight
{
	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	Vector3 Position;
	float Range;

	float Intensity;
	float Padding[3];
};

struct SpotLight
{
	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	Vector3 Position;
	float Range;

	Vector3 Direction;
	float Angle;

	float Intensity;
	float Padding[3];
};

class Lights
{
public:
	static Lights* Get();
	static void Create();
	static void Delete();
	
private:
	Lights();
	~Lights();

public:
	UINT PointLights(OUT PointLight* lights);
	void AddPointLight(PointLight& light);
	PointLight& GetPointLight(UINT index);

	UINT SpotLights(OUT SpotLight* lights);
	void AddSpotLight(SpotLight& light);
	SpotLight& GetSpotLight(UINT index);

	Color& FogColor() { return fogColor; }
	Vector2& FogDistance() { return fogDistance; }
	float& FogDensity() { return fogDensity; }
	UINT& FogType() { return fogType; }

private:
	static Lights* instance;

	UINT pointLightCount = 0;
	PointLight pointLights[MAX_POINT_LIGHT];

	UINT spotLightCount = 0;
	SpotLight spotLights[MAX_SPOT_LIGHT];


	Color fogColor = Color(1, 1, 1, 1);
	Vector2 fogDistance = Vector2(1, 105);
	float fogDensity = 0.5f;
	UINT fogType = 3;
};