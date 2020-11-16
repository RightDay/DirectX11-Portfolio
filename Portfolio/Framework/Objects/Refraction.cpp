#include "Framework.h"
#include "Refraction.h"

Refraction::Refraction(Shader * shader, wstring normalFile, UINT width, UINT height)
	: shader(shader)
{
	this->width = width > 0 ? width : (UINT)D3D::Width();
	this->height = height > 0 ? height : (UINT)D3D::Height();

	renderTarget = new RenderTarget(this->width, this->height);
	depthStencil = new DepthStencil(this->width, this->height);
	viewport = new Viewport((float)this->width, (float)this->height);


	sRefraction = shader->AsSRV("RefractionMap");
	
	normalMap = new Texture(normalFile);
	sNormalMap = shader->AsSRV("NormalMap");
}

Refraction::~Refraction()
{
	SafeDelete(renderTarget);
	SafeDelete(depthStencil);
	SafeDelete(viewport);

	SafeDelete(normalMap);
}

void Refraction::Update()
{
	
}

void Refraction::PreRender()
{
	renderTarget->Set(depthStencil);
	viewport->RSSetViewport();
}

void Refraction::Render()
{
	sNormalMap->SetResource(normalMap->SRV());
	sRefraction->SetResource(renderTarget->SRV());
}
