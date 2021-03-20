#include "Framework.h"
#include "Framework.h"
#include "HpBar.h"

HpBar::HpBar(Shader* shader, float w, float h)
	: w(w), h(h)
{
	Pass(0);
	SetShader(shader);

	for (UINT i = 0; i < MAX_MESH_INSTANCE; i++)
		D3DXMatrixIdentity(&worlds[i]);

	instanceBuffer = new VertexBuffer(worlds, MAX_MESH_INSTANCE, sizeof(Matrix), 1, true);
}

HpBar::~HpBar()
{
	SafeDelete(perFrame);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	for (Transform* transform : transforms)
		SafeDelete(transform);

	SafeDelete(instanceBuffer);
}

void HpBar::Create()
{
	vector<MeshVertex> v;
	v.push_back(MeshVertex(-w, -h, 0, 0, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(-w, +h, 0, 0, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(+w + width, -h, 0, 1, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(+w + width, +h, 0, 1, 0, 0, 0, -1, 1, 0, 0));

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

void HpBar::SetShader(Shader* shader)
{
	this->shader = shader;

	SafeDelete(perFrame);
	perFrame = new PerFrame(shader);
}

void HpBar::Update()
{
	perFrame->Update();
}

void HpBar::Render(UINT drawCount)
{
	instanceBuffer->Render();

	if (vertexBuffer == NULL || indexBuffer == NULL)
	{
		Create();

		vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(MeshVertex));
		indexBuffer = new IndexBuffer(indices, indexCount);
	}

	perFrame->Render();
	vertexBuffer->Render();
	indexBuffer->Render();

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	shader->DrawIndexedInstanced(0, pass, indexCount, transforms.size());
}

void HpBar::SetWidth(float w)
{
	width = w;
}

Transform* HpBar::AddTransform()
{
	Transform* transform = new Transform();
	transforms.push_back(transform);

	return transform;
}

void HpBar::UpdateTransforms()
{
	for (UINT i = 0; i < transforms.size(); i++)
		memcpy(worlds[i], transforms[i]->World(), sizeof(Matrix));

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MESH_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceBuffer->Buffer(), 0);
}
