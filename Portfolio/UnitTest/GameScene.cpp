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
	shader = new Shader(L"19_Terrain_Splatting.fx");

	sky = new CubeSky(L"Environment/GrassCube1024.dds");

	//Terrain
	{
		terrain = new Terrain(shader, L"Terrain/Gray512.png");
		terrain->BaseMap(L"Terrain/Dirt2.png");
		terrain->LayerMap(L"Terrain/Forest Floor.jpg", L"Terrain/Gray512.png");
		terrain->GetTransform()->Position(0, 0, 0);
		terrain->Pass(0);
		//terrain->Pass(1);
	}
	
	mutant = new Mutant();
	animators.push_back(mutant->GetModel());

	warrok = new Warrok();
	animators.push_back(warrok->GetModel());

	player = new Player();
	animators.push_back(player->GetModel());

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

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		mutant->GetHeight(terrain, i);
		warrok->GetHeight(terrain, i);

		if (player->IsIntersect(mutant->collider[i]))
		{
			mutant->minusHP(1);
		}
		if (player->IsIntersect(warrok->collider[i]))
		{
			warrok->minusHP(1);
		}
		if (mutant->IsIntersect(player->playerCollider, i))
		{
			player->minusHP(1);
		}
		if (warrok->IsIntersect(player->playerCollider, i))
		{
			player->minusHP(1);
		}
	}

	player->playerGetHeight(terrain);

	ImGui::Begin("TerrainLod");
	ImGui::Text("TerrainHeight : %f", terrain->GetHeight(player->GetModel()->GetTransform(0)->Position()));
	ImGui::End();
}

void GameScene::Render()
{
	sky->Render();
	Pass(0, 1, 2);
	terrain->Render();

	mutant->Render();
	warrok->Render();

	player->Render();
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
