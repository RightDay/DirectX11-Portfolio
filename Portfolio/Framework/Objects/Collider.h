#pragma once

struct ColliderObjectDesc
{
	class Transform* Init = NULL;
	class Transform* Transform = NULL;
	class Collider* Collider;
};

class Collider
{
public:
	struct Obb;

public:
	Collider(Transform* transform, Transform* init = NULL);
	~Collider();

	bool IsIntersect(Collider* other);

	void Update();
	void Render(Color& color);

	Transform* GetTransform() { return transform; }

private:
	void SetObb();

private:
	bool SeperatingPlane(Vector3& position, Vector3& direction, Obb& box1, Obb& box2);
	bool Collision(Obb& box1, Obb& box2);

	Vector3 Cross(Vector3& vec1, Vector3& vec2);

public:
	struct Obb
	{
		Vector3 Position;

		Vector3 AxisX;
		Vector3 AxisY;
		Vector3 AxisZ;

		Vector3 HalfSize;
	} obb;

private:
	Transform* init = NULL;
	Transform* transform;

	Vector3 lines[8];
};