#include "Framework.h"
#include "Render2D.h"

Render2D::Render2D(wstring shaderFile)
	: pass(0)
{
	if (shaderFile.length() < 1)
		shaderFile = L"29_Render2D.fx";

	shader = new Shader(shaderFile);

	transform = new Transform(shader);
	perFrame = new PerFrame(shader);


	D3DDesc desc = D3D::GetDesc();

	D3DXMatrixLookAtLH(&vpDesc.View, &Vector3(0, 0, -1), &Vector3(0, 0, 0), &Vector3(0, 1, 0));
	D3DXMatrixOrthoOffCenterLH(&vpDesc.Projection, 0, desc.Width, 0, desc.Height, -1, 1);

	vpBuffer = new ConstantBuffer(&vpDesc, sizeof(ViewProjectionDesc));
	sVpBuffer = shader->AsConstantBuffer("CB_Render2D");

	sDiffuseMap = shader->AsSRV("DiffuseMap");

	VertexTexture vertices[6];
	vertices[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[3].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[4].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[5].Position = Vector3(+0.5f, +0.5f, 0.0f);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 1);
	vertices[4].Uv = Vector2(0, 0);
	vertices[5].Uv = Vector2(1, 0);

	vertexBuffer = new VertexBuffer(vertices, 6, sizeof(VertexTexture));

}

Render2D::~Render2D()
{
	SafeDelete(shader);
	SafeDelete(transform);
	SafeDelete(perFrame);

	SafeDelete(vpBuffer);
	SafeDelete(vertexBuffer);
}

void Render2D::Update()
{
	transform->Update();
	perFrame->Update();
}

void Render2D::Render()
{
	transform->Render();
	perFrame->Render();

	vpBuffer->Apply();
	sVpBuffer->SetConstantBuffer(vpBuffer->Buffer());


	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	vertexBuffer->Render();

	shader->Draw(0, pass, 6);
}

void Render2D::SRV(ID3D11ShaderResourceView * srv)
{
	sDiffuseMap->SetResource(srv);
}
