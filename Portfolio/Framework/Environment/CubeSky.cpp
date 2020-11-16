#include "Framework.h"
#include "CubeSky.h"

CubeSky::CubeSky(wstring file, Shader * shader)
	: shader(shader)
{
	if (shader == NULL)
	{
		this->shader = new Shader(L"16_CubeSky.fxo");
		bCreateShader = true;
	}


	sphereRender = new MeshRender(this->shader, new MeshSphere(0.5f));
	sphereRender->AddTransform()->Scale(500, 500, 500);

	wstring temp = L"../../_Textures/" + file;
	Check(D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice(), temp.c_str(), NULL, NULL, &srv, NULL
	));

	sSrv = this->shader->AsSRV("SkyCubeMap");
}

CubeSky::~CubeSky()
{
	if (bCreateShader == true)
		SafeDelete(shader);

	SafeRelease(srv);
	SafeDelete(sphereRender);
}

void CubeSky::Update()
{
	Vector3 position;
	Context::Get()->GetCamera()->Position(&position);
	sphereRender->GetTransform(0)->Position(position);
	sphereRender->UpdateTransforms();
}

void CubeSky::Render()
{
	sSrv->SetResource(srv);

	sphereRender->Pass(pass);
	sphereRender->Render();
}
