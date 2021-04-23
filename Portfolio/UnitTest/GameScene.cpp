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
	((Freedom*)Context::Get()->GetCamera())->Speed(100, 2);
	shader = new Shader(L"19_Terrain_Splatting.fx");

	//Terrain
	{
		terrain = new Terrain(shader, L"Terrain/terrain_height.png");
		terrain->BaseMap(L"Terrain/Snow.jpg");
		terrain->LayerMap(L"Terrain/Rock (Basic).jpg", L"Terrain/terrain_height.png");
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

	Trees(20);
	
	AddWater(100.0f);
	sky = new CubeSky(L"Environment/SnowCube1024.dds", shader);

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

	trees->Update();

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		mutant->GetHeight(terrain, i);
		warrok->GetHeight(terrain, i);

		if (!player->bAttack)
		{
			mutant->bDamaged[i] = false;
			warrok->bDamaged[i] = false;
		}

		if (player->IsIntersect(mutant->collider[i]))
		{
			if (!mutant->bDamaged[i])
			{
				mutant->Damaged(i, player->bAttack);
				mutant->minusHP(i, 1);
			}
		}
		if (player->IsIntersect(warrok->collider[i]))
		{
			if (!warrok->bDamaged[i])
			{
				warrok->Damaged(i, player->bAttack);
				warrok->minusHP(i, 1);
			}
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

	static Vector3 waterPos = Vector3(0, 0, 0);
	static Vector3 waterScale = Vector3(0, 0, 0);

	ImGui::SliderFloat3("waterPos", waterPos, -500, 1000);
	ImGui::SliderFloat3("waterScale", waterScale, -500, 1000);
	//ImGui::InputFloat("waterPos.x", &waterPos.x);
	//ImGui::InputFloat("waterPos.y", &waterPos.y);
	//ImGui::InputFloat("waterPos.z", &waterPos.z);

	water->GetTransform()->Position(waterPos);
	water->GetTransform()->Scale(waterScale);

	water->Update();

	player->playerGetHeight(terrain);

	ImGui::Begin("TerrainLod");
	ImGui::Text("TerrainHeight : %f", terrain->GetHeight(player->GetModel()->GetTransform(0)->Position()));
	ImGui::End();
}

void GameScene::PreRender()
{
	//Reflaction
	{
		water->PreRender_Reflection();

		sky->Pass(7);
		sky->Render();

		Pass(8, 9, 10);

		mutant->Render();
		warrok->Render();

		player->Render();

		trees->Render();
	}

	//Refraction
	{
		water->PreRender_Refraction();

		sky->Pass(0);
		sky->Render();

		Pass(4, 5, 6);

		mutant->Render();
		warrok->Render();

		player->Render();

		trees->Render();
	}
}

void GameScene::Render()
{
	sky->Render();
	Pass(4, 5, 6);
	terrain->Render();

	mutant->Render();
	warrok->Render();

	player->Render();

	trees->Render();

	water->Pass(11);
	water->Render();
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

void GameScene::Trees(UINT num)
{
	shader = new Shader(L"43_Water.fx");
	trees = new ModelRender(shader);

	//trees->ReadMaterial(L"Trees/low_poly_tree");
	//trees->ReadMesh(L"Trees/low_poly_tree");
	trees->ReadMaterial(L"Trees/DeadTree1");
	trees->ReadMesh(L"Trees/DeadTree1");
	{
		Transform* transform = NULL;

		for (int i = 0; i < num; i++)
		{
			transform = trees->AddTransform();

			Vector3 randomVec3;
			randomVec3 = Math::RandomVec3(256.0f, 768.0f);
			randomVec3.y = 0.0f;

			transform->Position(randomVec3);
			transform->Scale(0.1f, 0.1f, 0.1f);
			//transform->Scale(20.0f, 20.0f, 20.0f);
		}
	}
	trees->UpdateTransforms();

	models.push_back(trees);
}

void GameScene::AddWater(float radius)
{
	shader = new Shader(L"43_Water.fxo");
	water = new Water(shader, radius);
	water->GetTransform()->Position(512.0f, -1.0f, 512.0f);
}
