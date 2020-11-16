#include "Framework.h"
#include "GBuffer.h"

GBuffer::GBuffer(Shader * shader, UINT width, UINT height)
	: shader(shader)
{
	this->width = width < 1 ? (UINT)D3D::Width() : width;
	this->height = height < 1 ? (UINT)D3D::Height() : height;

	diffuseRTV = new RenderTarget(this->width, this->height, DXGI_FORMAT_R8G8B8A8_UNORM);
	specularRTV = new RenderTarget(this->width, this->height, DXGI_FORMAT_R8G8B8A8_UNORM);
	emissiveRTV = new RenderTarget(this->width, this->height, DXGI_FORMAT_R8G8B8A8_UNORM);
	normalRTV = new RenderTarget(this->width, this->height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	tangentRTV = new RenderTarget(this->width, this->height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	depthStencil = new DepthStencil(this->width, this->height, true);
	viewport = new Viewport((float)this->width, (float)this->height);

	srvs[0] = depthStencil->SRV();
	srvs[1] = diffuseRTV->SRV();
	srvs[2] = specularRTV->SRV();
	srvs[3] = emissiveRTV->SRV(); 
	srvs[4] = normalRTV->SRV();
	srvs[5] = tangentRTV->SRV();
	

	sSrvs = shader->AsSRV("GBufferMaps");


	pointLightBuffer = new ConstantBuffer(&pointLightDesc, sizeof(PointLightDesc));
	sPointLightBuffer = shader->AsConstantBuffer("CB_Deffered_PointLight");

	spotLightBuffer = new ConstantBuffer(&spotLightDesc, sizeof(SpotLightDesc));
	sSpotLightBuffer = shader->AsConstantBuffer("CB_Deffered_SpotLight");

	
	CreateRasterizerState();
	CreateDepthStencilView();
	CreateDepthStencilState();

	sDSS = shader->AsDepthStencil("Deffered_DepthStencil_State");
	sRSS = shader->AsRasterizer("Deffered_Rasterizer_State");

	for (UINT i = 0; i < 5; i++)
	{
		render2D[i] = new Render2D();
		render2D[i]->GetTransform()->Position(100 + (float)i * 200, 100, 0);
		render2D[i]->GetTransform()->Scale(200, 200, 1);
	}

	render2D[0]->SRV(diffuseRTV->SRV());
	render2D[1]->SRV(specularRTV->SRV());
	render2D[2]->SRV(emissiveRTV->SRV());
	render2D[3]->SRV(normalRTV->SRV());
	render2D[4]->SRV(tangentRTV->SRV());
}

GBuffer::~GBuffer()
{
	SafeDelete(diffuseRTV);
	SafeDelete(specularRTV);
	SafeDelete(emissiveRTV);
	SafeDelete(normalRTV);
	SafeDelete(tangentRTV);
	SafeDelete(depthStencil);
	SafeDelete(viewport);

	SafeDelete(spotLightBuffer);
	SafeDelete(pointLightBuffer);
	
	SafeRelease(depthStencilReadOnly);

	SafeRelease(debugRSS);
	SafeRelease(lightRSS);
	
	SafeRelease(packDSS);
	SafeRelease(directionalLightDSS);
	SafeRelease(lightDSS);

	for (UINT i = 0; i < 5; i++)
		SafeDelete(render2D[i]);
}

void GBuffer::PreRender()
{
	RenderTarget* rtvs[5] = { diffuseRTV, specularRTV, emissiveRTV, normalRTV, tangentRTV };
	RenderTarget::Sets(rtvs, 5, depthStencil);
	viewport->RSSetViewport();

	sDSS->SetDepthStencilState(0, packDSS);
}

void GBuffer::Render(RenderTarget * rtv)
{
	D3D::Get()->SetRenderTarget(rtv->RTV(), depthStencilReadOnly);
	D3D::GetDC()->ClearDepthStencilView(depthStencilReadOnly, D3D11_CLEAR_DEPTH, 1, 0);

	
	sSrvs->SetResourceArray(srvs, 0, 6);

	D3D::GetDC()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);

	DirectionalLight();

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
	PointLights();
	SpotLights();
}

void GBuffer::PostRender()
{
	for (int i = 0; i < 5; i++)
	{
		render2D[i]->Update();
		render2D[i]->Render();
	}
}

void GBuffer::DirectionalLight()
{
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	sDSS->SetDepthStencilState(0, directionalLightDSS);

	shader->Draw(0, 6, 4);
}

void GBuffer::CalcPointLights(UINT count)
{
	for (UINT i = 0; i < count; i++)
	{
		Matrix S, T;
		float s = pointLightDesc.PointLight[i].Range;
		Vector3 t = pointLightDesc.PointLight[i].Position;

		D3DXMatrixScaling(&S, s, s, s);
		D3DXMatrixTranslation(&T, t.x, t.y, t.z);

		pointLightDesc.Projection[i] = S * T * Context::Get()->View() * Context::Get()->Projection();
	}
}

void GBuffer::PointLights()
{
	//ImGui::InputFloat("PointLight Factor", &pointLightDesc.TessFactor, 1.0f);
	sPointLightBuffer->SetConstantBuffer(pointLightBuffer->Buffer());

	
	UINT count = Lights::Get()->PointLights(pointLightDesc.PointLight);
	
	CalcPointLights(count);
	pointLightBuffer->Apply();

	if(bDrawDebug == true)
	{
		sRSS->SetRasterizerState(0, debugRSS);
		shader->Draw(0, 7, count * 2);
	}
	

	sRSS->SetRasterizerState(0, lightRSS);
	sDSS->SetDepthStencilState(0, lightDSS);

	shader->Draw(0, 8, count * 2);
}

void GBuffer::CalcSpotLights(UINT count)
{
	for (UINT i = 0; i < count; i++)
	{
		float angle = Math::ToRadian(spotLightDesc.SpotLight[i].Angle);

		spotLightDesc.Angle[i].x = cosf(angle);
		spotLightDesc.Angle[i].y = sinf(angle);


		Matrix S, R, T;
		float s = spotLightDesc.SpotLight[i].Range;
		Vector3 t = spotLightDesc.SpotLight[i].Position;

		D3DXMatrixScaling(&S, s, s, s);
		D3DXMatrixTranslation(&T, t.x, t.y, t.z);

		
		Vector3 direction = spotLightDesc.SpotLight[i].Direction;
		bool bUp = (direction.y > 1 - 1e-6f || direction.y < -1 + 1e-6f);
		Vector3 up = bUp ? Vector3(0, 0, direction.y) : Vector3(0, 1, 0);

		Vector3 right;
		D3DXVec3Cross(&right, &up, &direction);
		D3DXVec3Normalize(&right, &right);

		D3DXVec3Cross(&up, &direction, &right);
		D3DXVec3Normalize(&up, &up);

		D3DXMatrixIdentity(&R);
		for (int k = 0; k < 3; k++)
		{
			R.m[0][k] = right[k];
			R.m[1][k] = up[k];
			R.m[2][k] = direction[k];
		}

		spotLightDesc.Projection[i] = S * R * T * Context::Get()->View() * Context::Get()->Projection();
	}
}

void GBuffer::SpotLights()
{
	//ImGui::InputFloat("SpotLight Factor", &spotLightDesc.TessFactor, 1.0f);
	sSpotLightBuffer->SetConstantBuffer(spotLightBuffer->Buffer());


	UINT count = Lights::Get()->SpotLights(spotLightDesc.SpotLight);

	CalcSpotLights(count);
	spotLightBuffer->Apply();

	if (bDrawDebug == true)
	{
		sRSS->SetRasterizerState(0, debugRSS);
		shader->Draw(0, 9, count);
	}


	sRSS->SetRasterizerState(0, lightRSS);
	sDSS->SetDepthStencilState(0, lightDSS);

	shader->Draw(0, 10, count);
}

void GBuffer::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));

	desc.FillMode = D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_NONE;
	Check(D3D::GetDevice()->CreateRasterizerState(&desc, &debugRSS));

	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_FRONT;
	Check(D3D::GetDevice()->CreateRasterizerState(&desc, &lightRSS));
}

void GBuffer::CreateDepthStencilView()
{
	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;
	desc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;

	Check(D3D::GetDevice()->CreateDepthStencilView(depthStencil->BackBuffer(), &desc, &depthStencilReadOnly));
}

void GBuffer::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = TRUE;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	desc.StencilEnable = TRUE;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp =
	{
		D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS
	};
	desc.FrontFace = stencilMarkOp;
	desc.BackFace = stencilMarkOp;

	Check(D3D::GetDevice()->CreateDepthStencilState(&desc, &packDSS));


	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp2 =
	{
		D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL
	};
	desc.FrontFace = stencilMarkOp2;
	desc.BackFace = stencilMarkOp2;

	Check(D3D::GetDevice()->CreateDepthStencilState(&desc, &directionalLightDSS));


	desc.DepthFunc = D3D11_COMPARISON_GREATER;
	Check(D3D::GetDevice()->CreateDepthStencilState(&desc, &lightDSS));
}
