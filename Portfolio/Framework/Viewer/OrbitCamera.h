#pragma once
#include "Camera.h"

class OrbitCamera : public Camera
{
public:
	OrbitCamera();
	~OrbitCamera();

	void Update() override;

	void Speed(float move, float rotation = 2.0f);

	void SetTarget(Vector3* targetPos) { this->targetPos = targetPos; }

private:
	float move = 20.0f;
	float rotation = 10.0f;
	float distance = 50.0f;

	Vector3* targetPos;
	Vector3 rotationVec = Vector3(0, 0, 0);
	Vector3 positionVec = Vector3(0, 0, 0);
};