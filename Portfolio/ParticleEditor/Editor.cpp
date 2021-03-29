#include "stdafx.h"
#include "Editor.h"
#include "Utilities/Path.h"
#include "Utilities/Xml.h"

void Editor::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(23, 0, 0);
	Context::Get()->GetCamera()->Position(0, 32, -67);
	((Freedom *)Context::Get()->GetCamera())->Speed(20, 2);


	shader = new Shader(L"43_Water.fxo");
	shadow = new Shadow(shader, Vector3(0, 0, 0), 65);
	
	sky = new CubeSky(L"Environment/GrassCube1024.dds", shader);
	
	Mesh();

	UpdateParticleList();
	UpdateTextureList();
}

void Editor::Destroy()
{

}

void Editor::Update()
{
	OnGUI();


	Vector3 position;
	sphere->GetTransform(0)->Position(&position);

	if (Keyboard::Get()->Press('L'))
		position.x += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('J'))
		position.x -= 20 * Time::Delta();

	if (Keyboard::Get()->Press('I'))
		position.z += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('K'))
		position.z -= 20 * Time::Delta();

	if (Keyboard::Get()->Press('O'))
		position.y += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('U'))
		position.y -= 20 * Time::Delta();

	sphere->GetTransform(0)->Position(position);
	sphere->UpdateTransforms();


	sky->Update();
	
	sphere->Update();
	grid->Update();

	if (particle != NULL)
	{
		particle->Add(position);
		particle->Update();
	}
}

void Editor::PreRender()
{
	//Depth
	{
		shadow->PreRender();

		floor->Render();
		
		grid->Pass(1);
		grid->Render();

		brick->Render();
		sphere->Pass(1);
		sphere->Render();
	}
}

void Editor::Render()
{
	sky->Pass(0);
	sky->Render();

	floor->Render();
	grid->Pass(4);
	grid->Render();

	brick->Render();
	sphere->Pass(4);
	sphere->Render();

	if(particle != NULL)
		particle->Render();
}

void Editor::PostRender()
{
	
}

void Editor::Mesh()
{
	//Create Material
	{
		floor = new Material(shader);
		floor->DiffuseMap("Floor.png");
		floor->Specular(1, 1, 1, 15);
		floor->SpecularMap("Floor_Specular.png");
		floor->NormalMap("Floor_Normal.png");

		brick = new Material(shader);
		brick->DiffuseMap("Bricks.png");
		brick->Specular(1, 0.3f, 0.3f, 2);
		brick->SpecularMap("Bricks_Specular.png");
		brick->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
		brick->NormalMap("Bricks_Normal.png");
	}

	//Create Mesh
	{
		Transform* transform = NULL;

		sphere = new MeshRender(shader, new MeshSphere(0.5f));
		transform = sphere->AddTransform();
		transform->Position(0, 10, 0);
		transform->Scale(3, 3, 3);

		grid = new MeshRender(shader, new MeshGrid(5, 5));
		transform = grid->AddTransform();
		transform->Position(0, 0, 0);
		transform->Scale(12, 1, 12);
	}

	sphere->UpdateTransforms();
	grid->UpdateTransforms();
}

void Editor::OnGUI()
{
	float windowWidth = 500;

	float width = D3D::Width();
	float height = D3D::Height();

	bool bOpen = true;
	bOpen = ImGui::Begin("Particle", &bOpen);
	ImGui::SetWindowPos(ImVec2(width - windowWidth, 0));
	ImGui::SetWindowSize(ImVec2(windowWidth, height));
	{
		OnGUI_List();
		OnGUI_Setttings();
	}
	ImGui::End();
}

void Editor::OnGUI_List()
{
	if (ImGui::CollapsingHeader("Particle List", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (UINT i = 0; i < particleList.size(); i++)
		{
			if (ImGui::Button(String::ToString(particleList[i]).c_str(), ImVec2(200, 0)))
			{
				SafeDelete(particle);

				file = particleList[i];
				particle = new ParticleSystem(particleList[i]);
				maxParticle = particle->GetData().MaxParticles;
			}
		}//for(i)
	}//ImGui::CollapsingHeader
}

void Editor::OnGUI_Write()
{
	ImGui::Separator();

	if (ImGui::Button("WriteParticle"))
	{
		D3DDesc desc = D3D::GetDesc();

		Path::SaveFileDialog
		(
			file, L"Particle file\0*.xml", L"../../_Textures/Particles",
			bind(&Editor::WriteFile, this, placeholders::_1),
			desc.Handle
		);
	}
}

void Editor::OnGUI_Setttings()
{
	if (particle == NULL) return;

	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Particle Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Separator();

		ImGui::SliderInt("MaxParticles", (int *)&maxParticle, 1, 1000);

		if (ImGui::Button("Apply"))
		{
			particle->GetData().MaxParticles = maxParticle;
			particle->Reset();
		}
			
		ImGui::Separator();

		const char* types[] = { "Opaque", "Additive", "AlphaBlend" };
		ImGui::Combo("BlendType", (int *)&particle->GetData().Type, types, 3);

		ImGui::SliderFloat("Duration", &particle->GetData().Duration, 0.1f, 10.0f);
		ImGui::SliderFloat("DurationRandomness", &particle->GetData().DurationRandomness, 0.0f, 10.0f);

		ImGui::SliderFloat("EmitterVelocitySensitivity", &particle->GetData().EmitterVelocitySensitivity, 0.0f, 10.0f);

		ImGui::SliderFloat("MinHorizontalVelocity", &particle->GetData().MinHorizontalVelocity, -100.0f, 100.0f);
		ImGui::SliderFloat("MaxHorizontalVelocity", &particle->GetData().MaxHorizontalVelocity, -100.0f, 100.0f);

		ImGui::SliderFloat("MinVerticalVelocity", &particle->GetData().MinVerticalVelocity, -100.0f, 100.0f);
		ImGui::SliderFloat("MaxVerticalVelocity", &particle->GetData().MaxVerticalVelocity, -100.0f, 100.0f);
		
		ImGui::SliderFloat3("Gravity", particle->GetData().Gravity, -100, 100);

		ImGui::SliderFloat("EndVelocity", &particle->GetData().EndVelocity, -100.0f, 100.0f);

		ImGui::ColorEdit4("MinColor", particle->GetData().MinColor);
		ImGui::ColorEdit4("MaxColor", particle->GetData().MaxColor);

		ImGui::SliderFloat("MinRotateSpeed", &particle->GetData().MinRotateSpeed, -10, 10);
		ImGui::SliderFloat("MaxRotateSpeed", &particle->GetData().MaxRotateSpeed, -10, 10);
		
		ImGui::SliderFloat("MinStartSize", &particle->GetData().MinStartSize, 0, 500);
		ImGui::SliderFloat("MaxStartSize", &particle->GetData().MaxStartSize, 0, 500);

		ImGui::SliderFloat("MinEndSize", &particle->GetData().MinEndSize, 0, 500);
		ImGui::SliderFloat("MaxEndSize", &particle->GetData().MaxEndSize, 0, 500);

		ImGui::Spacing();
		OnGUI_Write();

		
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::CollapsingHeader("TextureList", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (wstring textureFile : textureList)
			{
				if (ImGui::Button(String::ToString(textureFile).c_str(), ImVec2(200, 0)))
					particle->SetTexture(textureFile);
			}//for(i)
		}		
	}
}

void Editor::UpdateParticleList()
{
	particleList.clear();
	Path::GetFiles(&particleList, L"../../_Textures/Particles/", L"*.xml", false);

	for (wstring& file : particleList)
		file = Path::GetFileNameWithoutExtension(file);
}

void Editor::UpdateTextureList()
{
	textureList.clear();

	vector<wstring> files;
	Path::GetFiles(&files, L"../../_Textures/Particles/", L"*.*", false);

	for (wstring file : files)
	{
		wstring ext = Path::GetExtension(file);
		transform(ext.begin(), ext.end(), ext.begin(), toupper);

		file = Path::GetFileName(file);
		if (ext == L"PNG" || ext == L"TGA" || ext == L"JPG")
			textureList.push_back(file);
	}
}

void Editor::WriteFile(wstring file)
{
	Xml::XMLDocument* document = new Xml::XMLDocument();

	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Particle");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);


	Xml::XMLElement* node = NULL;

	node = document->NewElement("BlendState");
	node->SetText((int)particle->GetData().Type);
	root->LinkEndChild(node);


	string textureFile = String::ToString(particle->GetData().TextureFile);
	String::Replace(&textureFile, "Particles/", "");

	node = document->NewElement("TextureFile");
	node->SetText(textureFile.c_str());
	root->LinkEndChild(node);


	node = document->NewElement("MaxParticles");
	node->SetText(particle->GetData().MaxParticles);
	root->LinkEndChild(node);


	node = document->NewElement("BlendSDurationtate");
	node->SetText(particle->GetData().Duration);
	root->LinkEndChild(node);

	node = document->NewElement("DurationRandomness");
	node->SetText(particle->GetData().DurationRandomness);
	root->LinkEndChild(node);

	node = document->NewElement("EmitterVelocitySensitivity");
	node->SetText(particle->GetData().EmitterVelocitySensitivity);
	root->LinkEndChild(node);

	
	node = document->NewElement("MinHorizontalVelocity");
	node->SetText(particle->GetData().MinHorizontalVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("MaxHorizontalVelocity");
	node->SetText(particle->GetData().MaxHorizontalVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("MinVerticalVelocity");
	node->SetText(particle->GetData().MinVerticalVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("MaxVerticalVelocity");
	node->SetText(particle->GetData().MaxVerticalVelocity);
	root->LinkEndChild(node);

	
	node = document->NewElement("Gravity");
	node->SetAttribute("X", particle->GetData().Gravity.x);
	node->SetAttribute("Y", particle->GetData().Gravity.y);
	node->SetAttribute("Z", particle->GetData().Gravity.z);
	root->LinkEndChild(node);

	node = document->NewElement("EndVelocity");
	node->SetText(particle->GetData().EndVelocity);
	root->LinkEndChild(node);


	node = document->NewElement("MinColor");
	node->SetAttribute("R", particle->GetData().MinColor.r);
	node->SetAttribute("G", particle->GetData().MinColor.g);
	node->SetAttribute("B", particle->GetData().MinColor.b);
	node->SetAttribute("A", particle->GetData().MinColor.a);
	root->LinkEndChild(node);

	node = document->NewElement("MaxColor");
	node->SetAttribute("R", particle->GetData().MaxColor.r);
	node->SetAttribute("G", particle->GetData().MaxColor.g);
	node->SetAttribute("B", particle->GetData().MaxColor.b);
	node->SetAttribute("A", particle->GetData().MaxColor.a);
	root->LinkEndChild(node);


	node = document->NewElement("MinRotateSpeed");
	node->SetText(particle->GetData().MinRotateSpeed);
	root->LinkEndChild(node);

	node = document->NewElement("MaxRotateSpeed");
	node->SetText(particle->GetData().MaxRotateSpeed);
	root->LinkEndChild(node);

	node = document->NewElement("MinStartSize");
	node->SetText((int)particle->GetData().MinStartSize);
	root->LinkEndChild(node);

	node = document->NewElement("MaxStartSize");
	node->SetText((int)particle->GetData().MaxStartSize);
	root->LinkEndChild(node);

	node = document->NewElement("MinEndSize");
	node->SetText((int)particle->GetData().MinEndSize);
	root->LinkEndChild(node);

	node = document->NewElement("MaxEndSize");
	node->SetText((int)particle->GetData().MaxEndSize);
	root->LinkEndChild(node);

	wstring folder = Path::GetDirectoryName(file);
	wstring fileName = Path::GetFileNameWithoutExtension(file);

	document->SaveFile(String::ToString(folder + fileName + L".xml").c_str());
	SafeDelete(document);

	UpdateParticleList();
}
