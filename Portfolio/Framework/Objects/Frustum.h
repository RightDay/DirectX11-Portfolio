#pragma once

class Frustum
{
public:
	Frustum(Camera* camera = NULL, Perspective* perspective = NULL);
	~Frustum();

	void Update();
	void Planes(Plane* planes);

	bool CheckPoint(Vector3& position);
	bool CheckCube(Vector3& center, Vector3& size);
	bool CheckCube(Vector3& center, float radius);

private:
	Plane planes[6];

	Camera* camera;
	Perspective* perspective;
};