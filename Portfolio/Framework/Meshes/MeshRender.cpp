#include "Framework.h"
#include "MeshRender.h"
#include "HpBar.h"

MeshRender::MeshRender(Shader * shader, Mesh * mesh)
	: mesh(mesh)
{
	Pass(0);
	mesh->SetShader(shader);

	for (UINT i = 0; i < MAX_MESH_INSTANCE; i++)
		D3DXMatrixIdentity(&worlds[i]);

	instanceBuffer = new VertexBuffer(worlds, MAX_MESH_INSTANCE, sizeof(Matrix), 1, true);
}

MeshRender::~MeshRender()
{
	for (Transform* transform : transforms)
		SafeDelete(transform);

	SafeDelete(instanceBuffer);
	SafeDelete(mesh);
}

void MeshRender::Update()
{
	mesh->Update();
}

void MeshRender::Render()
{
	instanceBuffer->Render();

	mesh->Render(transforms.size());
}

Transform * MeshRender::AddTransform()
{
	Transform* transform = new Transform();
	transforms.push_back(transform);

	return transform;
}

void MeshRender::UpdateTransforms()
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
