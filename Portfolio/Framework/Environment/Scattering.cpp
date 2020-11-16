#include "Framework.h"
#include "Scattering.h"

Scattering::Scattering(Shader * shader) 
	: shader(shader), width(256), height(256)
{
	mieTarget = new RenderTarget(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	rayleighTarget = new RenderTarget(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	depthStencil = new DepthStencil(width, height);
	viewport = new Viewport((float)width, (float)height);

	render2D = new Render2D();
	render2D->GetTransform()->Scale(200, 200, 1);
	render2D->GetTransform()->RotationDegree(0, 0, -90);

	CreateQuad();
}

Scattering::~Scattering()
{
	SafeDelete(render2D);

	SafeDelete(mieTarget);
	SafeDelete(rayleighTarget);
	SafeDelete(depthStencil);
	SafeDelete(viewport);

	SafeDelete(vertexBuffer);
}

void Scattering::Update()
{
	if (bDebug == true)
		render2D->Update();
}

void Scattering::PreRender()
{
	RenderTarget* rtvs[2];
	rtvs[0] = rayleighTarget;
	rtvs[1] = mieTarget;
	RenderTarget::Sets(rtvs, 2, depthStencil);

	viewport->RSSetViewport();


	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	vertexBuffer->Render();
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Draw(0, pass, 6);
}

void Scattering::PostRender()
{
	render2D->SRV(rayleighTarget->SRV());
	render2D->GetTransform()->Position(0 + 100, D3D::Height() - 100, 0);
	render2D->Update();
	render2D->Render();

	render2D->SRV(mieTarget->SRV());
	render2D->GetTransform()->Position(200 + 100, D3D::Height() - 100, 0);
	render2D->Update();
	render2D->Render();
}

void Scattering::CreateQuad()
{
	VertexTexture vertices[6];
	vertices[0].Position = Vector3(-1.0f, -1.0f, 0.0f);
	vertices[1].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[2].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[3].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[4].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[5].Position = Vector3(+1.0f, +1.0f, 0.0f);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 1);
	vertices[4].Uv = Vector2(0, 0);
	vertices[5].Uv = Vector2(1, 0);

	vertexBuffer = new VertexBuffer(vertices, 6, sizeof(VertexTexture));
}
