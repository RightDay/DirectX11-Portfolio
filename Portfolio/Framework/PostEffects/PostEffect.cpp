#include "Framework.h"
#include "PostEffect.h"

PostEffect::ValueDesc PostEffect::valueDesc;

PostEffect::PostEffect(wstring shaderFile, ID3D11ShaderResourceView ** gBufferSrvs)
	: shaderFile(shaderFile), pass(0), gBufferSrvs(gBufferSrvs)
{
	shader = new Shader(shaderFile);

	transform = new Transform(shader);
	perFrame = new PerFrame(shader);


	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_PostEffect");

	valueBuffer = new ConstantBuffer(&valueDesc , sizeof(ValueDesc));
	sValueBuffer = shader->AsConstantBuffer("CB_PostEffectValue");

	
	Vertex vertices[6];
	vertices[0].Position = Vector3(-1, -1, 0);
	vertices[1].Position = Vector3(-1, +1, 0);
	vertices[2].Position = Vector3(+1, -1, 0);
	vertices[3].Position = Vector3(+1, -1, 0);
	vertices[4].Position = Vector3(-1, +1, 0);
	vertices[5].Position = Vector3(+1, +1, 0);

	vertexBuffer = new VertexBuffer(vertices, 6, sizeof(Vertex));


	for (UINT i = 0; i < MAX_POSTEFFECT_SRVS; i++)
		srvs[i] = NULL;

	sSrvs = shader->AsSRV("Maps");

	transform->Scale(D3D::Width(), D3D::Height(), 1);
	transform->Position(D3D::Width() * 0.5f, D3D::Height() * 0.5f, 1);


	sGBufferSrvs = shader->AsSRV("GBufferMaps");


	for (UINT i = 0; i < 4; i++)
		renderTarget[i] = new RenderTarget();
}

PostEffect::~PostEffect()
{
	SafeDelete(shader);
	
	SafeDelete(transform);
	SafeDelete(perFrame);
	
	SafeDelete(vertexBuffer);
	SafeDelete(buffer);

	for (UINT i = 0; i < 4; i++)
		SafeDelete(renderTarget[i]);
}

void PostEffect::Update()
{
	transform->Update();
	perFrame->Update();
}

void PostEffect::PreRender(DepthStencil * depthStencil)
{
	renderTarget[0]->Set(depthStencil);
}

void PostEffect::Render()
{
	transform->Render();
	perFrame->Render();


	Vector3 scale;
	transform->Scale(&scale);
	desc.PixelSize = Vector2(1.0f / scale.x, 1.0f / scale.y);

	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	valueBuffer->Apply();
	sValueBuffer->SetConstantBuffer(valueBuffer->Buffer());


	sSrvs->SetResourceArray(srvs, 0, MAX_POSTEFFECT_SRVS);
	sGBufferSrvs->SetResourceArray(gBufferSrvs, 0, 6);

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	vertexBuffer->Render();

	shader->Draw(technique, pass, 6);
}

void PostEffect::Srv(UINT index, ID3D11ShaderResourceView * srv)
{
	srvs[index] = srv;
}
