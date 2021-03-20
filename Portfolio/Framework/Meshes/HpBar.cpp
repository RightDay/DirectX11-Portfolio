#include "Framework.h"
#include "HpBar.h"

HpBar::HpBar(float w, float h)
	: w(w), h(h)
{

}

HpBar::~HpBar()
{

}

void HpBar::Create()
{
	vector<MeshVertex> v;
	v.push_back(MeshVertex(-w, -h, 0, 0, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(-w, +h, 0, 0, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(+w, -h, 0, 1, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(+w, +h, 0, 1, 0, 0, 0, -1, 1, 0, 0));

	vertices = new MeshVertex[v.size()];
	vertexCount = v.size();

	copy
	(
		v.begin(), v.end(),
		stdext::checked_array_iterator<MeshVertex*>(vertices, vertexCount)
	);

	indexCount = 6;
	indices = new UINT[indexCount]{ 0, 1, 2, 2, 1, 3 };
}