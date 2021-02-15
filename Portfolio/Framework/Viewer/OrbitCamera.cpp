#include "Framework.h"
#include "OrbitCamera.h"

OrbitCamera::OrbitCamera()
	: Camera()
{
}

OrbitCamera::~OrbitCamera()
{
}

void OrbitCamera::Update()
{
	Vector3 f = Forward();
	Vector3 u = Up();
	Vector3 r = Right();
	Vector3 fu = Vector3(0, -1, 1);

	//Move
	{
		Vector3 P;
		Position(&P);

		P = *targetPos - fu * distance;

		Position(P);
	}

	if (Mouse::Get()->Press(1))
	{
		//Rotation
		Vector3 R;
		Rotation(&R);

		Vector3 val = Mouse::Get()->GetMoveValue();
		R.x += val.y * rotation * Time::Delta();
		R.y += val.x * rotation * Time::Delta();
		R.z = 0.0f;

		Rotation(R);
	}

	if (Keyboard::Get()->Press('Z'))
	{
		distance += 5.0f * Time::Delta();
	}
	if (Keyboard::Get()->Press('C') && distance > 1.0f)
	{
		distance -= 5.0f* Time::Delta();
	}
}

void OrbitCamera::Speed(float move, float rotation)
{
	this->move = move;
	this->rotation = rotation;
}
