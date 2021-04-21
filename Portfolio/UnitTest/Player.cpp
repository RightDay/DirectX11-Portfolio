#include "stdafx.h"
#include "Player.h"
#include "PlayerState.h"
#include "HpBar.h"

Player::Player()
{
	CreateArcherModel();

	model = archer;

	playerPos = Vector3(512, 0.0f, 512);
	
	attachBone = 73;

	bAttack = true;

	//Hp Bar
	hp = 15;
	minusHpScale = hpGaugeScale / hp;
	minusHpPosition = hpGaugeScale / hp / 2.0f;

	//model->GetTransform(0)->Position(playerPos);
	CreatePlayerCollider();
	CreateHpBar();
	AddPartcle();
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

	PlayerControl();
	state->Update(*this);

	Move();
	Rotation();

	//static Vector3 thisPos = Vector3(0.0f, 0.0f, 0.0f);
	//model->GetTransform(0)->Position(&thisPos);
	model->UpdateTransforms();

	controlHpBar();

	hpBar->Update();
	hpGauge->Update();

	ImGui::Text("HP : %d", hp);

	if (particle != NULL)
	{
		particle->Add(playerPos);
		particle->Update();
	}
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

	hpBar->Render();
	hpGauge->Render();

	if (bAttack)
		particle->Render();
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
	archer->ReadClip(L"Archer/Dash_Attack");
	archer->ReadClip(L"Archer/Jump");
	archer->ReadClip(L"Archer/Hip_Hop_Dancing");

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
		playerCollider->Init->Position(0.0f, 100.0f, 0.0f);
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
	
	ImGui::Begin("Collider");
	ImGui::Text("Collider position %f", playerCollider->Collider->GetTransform()->Position().x);
	ImGui::End();
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
	ImGui::Begin("Player dir");
	ImGui::Text("Forward : %f %f %f", playerForward.x, playerForward.y, playerForward.z);
	ImGui::Text("Right : %f %f %f", playerRight.x, playerRight.y, playerRight.z);
	ImGui::End();
	moveVertical = playerForward * velocity * Time::Delta() * 2.0f;
	moveHorizontal = playerRight * velocity * Time::Delta() * 2.0f;

	if (isMove == false) return;

	model->GetTransform(0)->Position(playerPos);
}

void Player::Rotation()
{
	//Quaternion targetR;
	//Quaternion outR;
	//D3DXQuaternionNormalize(&outR, &outR);
	//D3DXQuaternionNormalize(&targetR, &targetR);
	//D3DXQuaternionToAxisAngle(&targetR, &Vector3(0, 1, 0), &angle);
	//D3DXQuaternionSlerp(&outR, &outR, &targetR, 10.0f * Time::Delta());
	
	//-------------------------------------------------------------------------------------------------
	Vector3 outRot;
	Vector3 targetRot;
	targetRot = Vector3(0.0f, angle, 0.0f);
	
	model->GetTransform(0)->Rotation(targetRot);
	ImGui::Begin("Player Status");
	ImGui::Text("Player Angle : %f %f %f", model->GetTransform(0)->Rotation().y);
	ImGui::Text("Angle : %f", angle);
	ImGui::End();
}

void Player::PlayerControl()
{
	//Player move forward
	if (Keyboard::Get()->Down('W'))
	{
		handleInput(DOWN_W);
	}

	//Player move backward
	if (Keyboard::Get()->Down('S'))
	{
		handleInput(DOWN_S);
	}

	//Player move right
	if (Keyboard::Get()->Down('D'))
	{
		handleInput(DOWN_D);
	}

	//Player move left
	if (Keyboard::Get()->Down('A'))
	{
		handleInput(DOWN_A);
	}

	//Up Key
	if (Keyboard::Get()->Up('W'))
	{
		handleInput(UP_W);
	}

	//Player move backward
	if (Keyboard::Get()->Up('S'))
	{
		handleInput(UP_S);
	}

	//Player move right
	if (Keyboard::Get()->Up('D'))
	{
		handleInput(UP_D);
	}

	//Player move left
	if (Keyboard::Get()->Up('A'))
	{
		handleInput(UP_A);
	}

	//Player rotation left
	if (Keyboard::Get()->Press('Q'))
	{
		RotationAngle(-1.0f);
	}

	//Player rotation right
	if (Keyboard::Get()->Press('E'))
	{
		RotationAngle(+1.0f);
	}

	//Player attack
	if (Keyboard::Get()->Down('F'))
	{
		handleInput(ATTACK);
	}
	
	if (Keyboard::Get()->Down('R'))
	{
		handleInput(DASH_ATTACK);
	}


	handleInput(INPUT_NULL);
}

void Player::controlHpBar()
{
	static Vector3 pos = Vector3(0, 0, 0);
	static Vector3 sca = Vector3(0, 0, 0);

	hpGauge->GetTransform()->Position(&pos);
	hpGauge->GetTransform()->Scale(&sca);

	if (hp < 0)
		return;

	ImGui::Begin("hp");

	ImGui::SliderFloat3("HpGauge pos", pos, 0, 800);
	ImGui::SliderFloat3("HpGauge sca", sca, 0, 800);
	static UINT prevHP = hp;
	if (prevHP > hp)
	{
		hpGauge->GetTransform()->Position(pos.x - minusHpPosition, pos.y, pos.z);

		prevHP = hp;
	}
	ImGui::End();
	//hpGauge->GetTransform()->Position(pos.x, pos.y, pos.z);
	hpGauge->GetTransform()->Scale(hp * minusHpScale, sca.y, sca.z);
}

void Player::AddPartcle()
{
	particleTexture = L"Test";
	particle = new ParticleSystem(particleTexture);
	//particle->SetTexture(particleTexture);
}

void Player::AddParticleTexture()
{
}

void Player::CreateHpBar()
{
	Texture* T_hpBar = new Texture(L"hp_bar.png");
	hpBar = new Render2D();
	hpBar->GetTransform()->Position(200.0f, 650.0f, 0);
	hpBar->GetTransform()->Scale(400.0f, 28.0f, 1);
	hpBar->SRV(T_hpBar->SRV());

	Texture* T_hpGauge = new Texture(L"Red.png");
	hpGauge = new Render2D();
	hpGauge->GetTransform()->Position(200.0f, 650.5f, 0);
	hpGauge->GetTransform()->Scale(hp * (hpGaugeScale / hp), 20.0f, 1);
	hpGauge->SRV(T_hpGauge->SRV());
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

void Player::RotationAngle(float angle)
{
	this->angle = angle;
}

bool Player::IsIntersect(ColliderObjectDesc* other)
{
	if (bAttack == false) return false;

	if (swordCollider->Collider->IsIntersect(other->Collider))
	{
		//bAttack = false;

		return true;
	}
	ImGui::Text("Player : bAttack : %d", bAttack);
	return false;
}

void Player::playerGetHeight(Terrain* terrain)
{
	playerPos.y = terrain->GetHeight(playerPos);
}
