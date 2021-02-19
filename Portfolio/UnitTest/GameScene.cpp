#include "stdafx.h"
#include "Player.h"
#include "GameScene.h"

void GameScene::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(32, 0, 0);
	//Context::Get()->GetCamera()->Position(0, 80, -30);
	//((Freedom*)Context::Get()->GetCamera())->Speed(100, 2);
	shader = new Shader(L"47_TerrainLod.fx");

	sky = new CubeSky(L"Environment/GrassCube1024.dds");

	//Terrain
	{
		TerrainLod::InitializeDesc desc =
		{
			shader,
			L"Terrain/Gray512.png",
			1.0f, 16, 5
		};

		terrain = new TerrainLod(desc);
		terrain->BaseMap(L"Terrain/Dirt2.png");
		terrain->LayerMap(L"Terrain/Forest Floor.jpg", L"Terrain/Gray512.png");
		terrain->NormalMap(L"Terrain/Dirt_Normal.png");
		terrain->Pass(0);
		//terrain->Pass(1);
	}

	CreateArcherModel();
	player = new Player(archer);

	((OrbitCamera*)Context::Get()->GetCamera())->SetTarget(player->GetPlayerPos());
}
void GameScene::Destroy()
{
	SafeDelete(terrain);
}

void GameScene::Update()
{
	sky->Update();
	terrain->Update();
	player->Update();
}

void GameScene::Render()
{
	sky->Render();
	Pass(0, 1, 2);
	terrain->Render();
	player->Render();
}

void GameScene::CreateArcherModel()
{
	modelShader = new Shader(L"27_Animation.fxo");
	archer = new ModelAnimator(modelShader);

	archer->ReadMaterial(L"Archer/Archer");
	archer->ReadMesh(L"Archer/Archer");

	archer->ReadClip(L"Archer/Idle");
	archer->ReadClip(L"Archer/Running");
	archer->ReadClip(L"Archer/Hip_Hop_Dancing");
	archer->ReadClip(L"Archer/Jump");

	archer->PlayClip(0, 0, 1.0f);

	Transform* transform = NULL;

	transform = archer->AddTransform();
	transform->Position(0.0f, 0.0f, 0.0f);
	transform->RotationDegree(0.0f, 180.0f, 0.0f);
	transform->Scale(0.075f, 0.075f, 0.075f);

	archer->UpdateTransforms();

	animators.push_back(archer);

	//Attach weapon
	{
		weapon = new Model();
		weapon->ReadMaterial(L"Weapon/Sword");
		weapon->ReadMesh(L"Weapon/Sword");

		Transform attachHand;

		attachHand.Scale(1.3f, 1.3f, 1.3f);
		attachHand.RotationDegree(0.0f, 0.0f, 90.0f);
		attachHand.Position(8.0f, 10.0f, -7.0f);

		archer->GetModel()->Attach(modelShader, weapon, 37, &attachHand);
	}
}


void GameScene::Pass(UINT mesh, UINT model, UINT anim)
{
	for (MeshRender* temp : meshes)
		temp->Pass(mesh);

	for (ModelRender* temp : models)
		temp->Pass(model);

	for (ModelAnimator* temp : animators)
		temp->Pass(anim);
}
