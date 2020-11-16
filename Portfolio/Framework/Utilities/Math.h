#pragma once
class Math
{
public:
	static const float PI;
	static const float EPSILON;

	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static int Random(int r1, int r2);
	static float Random(float r1, float r2);
	
	static D3DXVECTOR2 RandomVec2(float r1, float r2);
	static D3DXVECTOR3 RandomVec3(float r1, float r2);
	static D3DXCOLOR RandomColor3();
	static D3DXCOLOR RandomColor4();

	static bool IsZero(float a);
	static bool IsOne(float a);


	static int Clamp(int value, int min, int max);
	static float Clamp(float value, float min, float max);
	static LONG Clamp(LONG value, LONG min, LONG max);
	static UINT Clamp(UINT value, UINT min, UINT max);

	static float Lerp(float value1, float value2, float t);
	static void LerpMatrix(OUT D3DXMATRIX& out, const D3DXMATRIX& m1, const D3DXMATRIX& m2, float amount);

	static D3DXQUATERNION LookAt(const D3DXVECTOR3& origin, const D3DXVECTOR3& target, const D3DXVECTOR3& up);
	static float Gaussian(float val, UINT blurCount);

	static void MatrixDecompose(const D3DXMATRIX& m, OUT Vector3& S, OUT Vector3& R, OUT Vector3& T);
};