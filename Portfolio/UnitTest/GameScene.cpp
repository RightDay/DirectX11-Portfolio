#include "stdafx.h"
#include "Player.h"
#include "GameScene.h"

void GameScene::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(32, 0, 0);
	Context::Get()->GetCamera()->Position(0, 10, -30);
	((Freedom*)Context::Get()->GetCamera())->Speed(100, 2);

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
}
void GameScene::Destroy()
{
	SafeDelete(terrain);
}

void GameScene::Update()
{
	playerPos = player->GetPlayerPos();
	Context::Get()->GetCamera()->Position(playerPos.x, playerPos.y + 50.0f, playerPos.z - 50.0f);

	sky->Update();
	terrain->Update();
	player->Update();
}

void GameScene::Render()
{
	sky->Render();
	//Pass(0, 1, 2);
	terrain->Render();
	player->Render();
}

//Player Class에 있는 Pass를 가져와 구현 예정
//Player에 있는 Animators를 옮기는 작업 필요
//void GameScene::Pass(UINT mesh, UINT model, UINT anim)
//{
//	for (MeshRender* temp : meshes)
//		temp->Pass(mesh);
//
//	for (ModelRender* temp : models)
//		temp->Pass(model);
//
//	for (ModelAnimator* temp : animators)	//TODO: pass
//		temp->Pass(anim);
//}
