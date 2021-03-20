#include "stdafx.h"
#include "Player.h"
#include "PlayerState.h"
#include "HpBar.h"

Player::Player()
{
	CreateArcherModel();

	model = archer;

	playerPos = Vector3(0, 2.0f, 0);

	attachBone = 73;

	bAttack = true;

	hp = 10;

	gridShader = new Shader(L"27_Mesh.fx");

	//Material(hp)
	hpBG = new Material(gridShader);
	hpBG->DiffuseMap("hp_bg.png");

	//Grid
	hp_bg = new MeshRender(gridShader, new MeshQuad());
	hp_bg->AddTransform();
	hp_bg->GetTransform(0)->Scale(10, 1, 1);
	hp_bg->GetTransform(0)->RotationDegree(-90, 0, 0);
	hp_bg->UpdateTransforms();
	hp_bg->Pass(1);

	//Quad
	hpBar = new Material(gridShader);
	hpBar->DiffuseMap("Red.png");

	hp_bar = new HpBar(gridShader, 0.5f, 0.5f);
	hp_bar->AddTransform();
	hp_bar->GetTransform(0)->Scale(1, 1, 1);
	hp_bar->GetTransform(0)->RotationDegree(-90, 0, 0);
	hp_bar->UpdateTransforms();
	hp_bar->Pass(1);

	CreatePlayerCollider();
	state = new StandingState();
}

Player::Player(ModelAnimator* model)
	:model(model)
{
	state = new StandingState();
}

Player::~Player()
{
}

void Player::Update()
{
	model->Update();

	AttachCollider();
	//if (bAttack == true)
		AttachSwordCollider();

	state->Update(*this);

	PlayerControl();

	Move();
	Rotation();

	static Vector3 thisPos = Vector3(0.0f, 0.0f, 0.0f);
	model->GetTransform(0)->Position(&thisPos);
	model->UpdateTransforms();
	
	//Grid
	Vector3 camRot;
	Context::Get()->GetCamera()->RotationDegree(&camRot);
	static Vector3 gridScale = Vector3(10.0f, 1.0f, 1.0f);
	static Vector3 gridPosition = Vector3(0.0f, 0.0f, 0.05f);
	static float healthPoint = 0.0f;
	ImGui::Begin("Grid");
	ImGui::SliderFloat3("grid scale", gridScale, 0, 10.0f);
	ImGui::SliderFloat3("grid position", gridPosition, 0, 10.0f);
	ImGui::SliderFloat("HpBar", &healthPoint, -10.0f, 10.0f);
	ImGui::End();

	hp_bar->GetTransform(0)->Scale(gridScale);
	hp_bar->GetTransform(0)->Position(playerPos.x + gridPosition.x, playerPos.y + 20.0f, playerPos.z + gridPosition.z);
	hp_bar->GetTransform(0)->RotationDegree(camRot.x, camRot.y, 0.0f);
	hp_bar->SetWidth(healthPoint);
	hp_bar->Update();
	hp_bar->UpdateTransforms();

	ImGui::Text("HP : %d", hp);
}

void Player::Render()
{
	//Insert Shader Pass
	playerCollider->Collider->Render(Color(0, 0, 1, 1));
	if (bAttack == true)
	{
		swordCollider->Collider->Render(Color(0, 1, 0, 1));
	}

	model->Render();

	hpBG->Render();
	hp_bg->Render();

	hpBar->Render();
	hp_bar->Render(1);
}

void Player::Jump()
{
}

void Player::Run()
{
}

void Player::Attack()
{
}

void Player::handleInput(Input input)
{
	PlayerState* state_ = state->handleInput(*this, input);
	if (state_ != NULL)
	{
		delete state;
		state = state_;

		state->Enter(*this);
	}
}

void Player::CreateArcherModel()
{
	modelShader = new Shader(L"27_Animation.fxo");
	archer = new ModelAnimator(modelShader);

	archer->ReadMaterial(L"Archer/Archer");
	archer->ReadMesh(L"Archer/Archer");

	archer->ReadClip(L"Archer/Idle");
	archer->ReadClip(L"Archer/Running");
	archer->ReadClip(L"Archer/Attack");
	archer->ReadClip(L"Archer/Hip_Hop_Dancing");
	archer->ReadClip(L"Archer/Jump");

	archer->PlayClip(0, 0, 1.0f);

	Transform* transform = NULL;

	transform = archer->AddTransform();
	transform->Scale(0.075f, 0.075f, 0.075f);
	transform->RotationDegree(0.0f, 180.0f, 0.0f);
	transform->Position(0.0f, 0.0f, 0.0f);

	archer->UpdateTransforms();

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

void Player::CreatePlayerCollider()
{
	//Player Collider
	{
		playerCollider = new ColliderObjectDesc();
		playerCollider->Init = new Transform();
		playerCollider->Init->Position(playerPos.x, playerPos.y + 80, playerPos.z);
		playerCollider->Init->Scale(60.0f, 170.0f, 70.0f);

		playerCollider->Transform = new Transform();
		playerCollider->Collider = new Collider(playerCollider->Transform, playerCollider->Init);
	}

	//Sword Collider
	{
		swordCollider = new ColliderObjectDesc();
		swordCollider->Init = new Transform();

		//swordCollider->Init->Position(-8.0f, 0.0f, -24.923f);
		//swordCollider->Init->Scale(10.0f, 10.0f, 85.0f);

		swordCollider->Transform = new Transform();
		swordCollider->Collider = new Collider(swordCollider->Transform, swordCollider->Init);
	}
}

void Player::AttachCollider()
{
	Matrix attachPlayer = model->GetTransform(0)->World();
	playerCollider->Collider->GetTransform()->World(attachPlayer);
	
	playerCollider->Collider->Update();
}

void Player::AttachSwordCollider()
{
	if (swordCollider != NULL)
	{
		Matrix attach = model->GetAttachTransform(0, attachBone);

		static Vector3 colliderScale = Vector3(10.0f, 10.0f, 55.0f);
		static Vector3 colliderRotationDegree = Vector3(0.0f, 0.0f, 0.0f);
		static Vector3 colliderPosition = Vector3(-10.0f, -5.0f, -38.0f);

		ImGui::SliderFloat3("ColliderScale", colliderScale, 00.0f, 300.0f);
		swordCollider->Init->Scale(colliderScale);

		ImGui::SliderFloat3("ColliderRotation", colliderRotationDegree, -180.0f, 180.0f);
		swordCollider->Init->RotationDegree(colliderRotationDegree);


		ImGui::SliderFloat3("ColliderPosition", colliderPosition, -300.0f, 300.0f);
		swordCollider->Init->Position(colliderPosition);

		swordCollider->Collider = new Collider(swordCollider->Transform, swordCollider->Init);

		swordCollider->Collider->GetTransform()->World(attach);
		swordCollider->Collider->Update();
	}
}

void Player::ControlCollider(ColliderObjectDesc* collider)
{
	//Control playerColliderScale
	static Vector3 colliderScale = Vector3(1.0f, 1.0f, 1.0f);
	ImGui::SliderFloat3("PlayerCollider", colliderScale, 0.0f, 600.0f);
	collider->Init->Scale(colliderScale);

	collider->Collider = new Collider(collider->Transform, collider->Init);
}

void Player::Move()
{
	if (Mouse::Get()->Down(1) == true) return;

	playerForward = model->GetTransform(0)->Forward();
	playerRight = model->GetTransform(0)->Right();
	
	moveVertical = playerForward * velocity * Time::Delta() * 2.0f;
	moveHorizontal = playerRight * velocity * Time::Delta() * 2.0f;

	if (isMove == false) return;

	model->GetTransform(0)->Position(playerPos);
}

void Player::Rotation()
{
	playerRot = Vector3(0.0f, angle, 0.0f);

	model->GetTransform(0)->RotationDegree(playerRot);
}

void Player::PlayerControl()
{
	//Player move forward
	if (Keyboard::Get()->Press('W'))
	{
		handleInput(PRESS_W);
	}

	//Player move backward
	if (Keyboard::Get()->Press('S'))
	{
		handleInput(PRESS_S);
	}

	//Player move right
	if (Keyboard::Get()->Press('D'))
	{
		handleInput(PRESS_D);
	}

	//Player move left
	if (Keyboard::Get()->Press('A'))
	{
		handleInput(PRESS_A);
	}

	//Player rotation left
	if (Keyboard::Get()->Press('Q'))
	{
		playerRotationAngle(-1.0f);
	}

	//Player rotation right
	if (Keyboard::Get()->Press('E'))
	{
		playerRotationAngle(+1.0f);
	}

	if (Keyboard::Get()->Up('W') || Keyboard::Get()->Up('S') || Keyboard::Get()->Up('A') || Keyboard::Get()->Up('D'))
	{
		handleInput(RELEASE_MOVE);
	}

	//Player attack
	if (Keyboard::Get()->Down('F'))
	{
		handleInput(ATTACK);
	}

	handleInput(INPUT_NULL);
}

void Player::playerMovePos(Vector3 pos, bool plus)
{
	if (plus)
	{
		playerPos += pos;
	}
	else
	{
		playerPos -= pos;
	}
}

void Player::playerRotationAngle(float rotationAngle)
{
	angle += rotationAngle;
}

bool Player::IsIntersect(ColliderObjectDesc* other)
{
	if (bAttack == false) return false;

	if (swordCollider->Collider->IsIntersect(other->Collider))
	{
		bAttack = false;

		return true;
	}
	ImGui::Text("Player : bAttack : %d", bAttack);
	return false;
}
