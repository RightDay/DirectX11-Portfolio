#include "stdafx.h"
#include "Player.h"
#include "Mutant.h"
#include "Warrok.h"
#include "GameScene.h"

void GameScene::Initialize()
{
	srand(time(NULL));
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

	player = new Player();
	animators.push_back(player->GetModel());
	
	mutant = new Mutant();
	animators.push_back(mutant->GetModel());

	warrok = new Warrok();
	animators.push_back(warrok->GetModel());

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

	mutant->Update();
	mutant->Patrol(player->GetModel());

	warrok->Update();
	warrok->Patrol(player->GetModel());
}

void GameScene::Render()
{
	sky->Render();
	Pass(0, 1, 2);
	terrain->Render();

	player->Render();

	mutant->Render();
	warrok->Render();
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
