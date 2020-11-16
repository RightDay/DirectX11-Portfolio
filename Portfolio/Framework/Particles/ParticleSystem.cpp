#include "Framework.h"
#include "ParticleSystem.h"
#include "Utilities/Xml.h"
#include "Utilities/Path.h"

ParticleSystem::ParticleSystem(wstring file)
	: Renderer(L"48_Particle.fxo"), file(file)
{
	ReadFile(L"../../_Textures/Particles/" + file + L".xml");

	
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Particle");

	sMap = shader->AsSRV("ParticleMap");

	Reset();
}

ParticleSystem::~ParticleSystem()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(buffer);
	SafeDelete(map);
}

void ParticleSystem::Update()
{
	Super::Update();

	
	currentTime += Time::Delta();

	Activate();
	Deactivate();

	if (activeCount == freeCount)
		currentTime = 0.0f;


	desc.MinColor = data.MinColor;
	desc.MaxColor = data.MaxColor;

	desc.Gravity = data.Gravity;
	desc.EndVelocity = data.EndVelocity;

	desc.RotateSpeed = Vector2(data.MinRotateSpeed, data.MaxRotateSpeed);
	desc.StartSize = Vector2(data.MinStartSize, data.MaxStartSize);
	desc.EndSize = Vector2(data.MinEndSize, data.MaxEndSize);

	desc.Duration = data.Duration;
	desc.DurationRandomness = data.DurationRandomness;
}

void ParticleSystem::Render()
{
	Super::Render();

	if (newCount != freeCount)
		MapVertices();

	desc.CurrentTime = currentTime;
	
	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	sMap->SetResource(map->SRV());


	UINT pass = (UINT)data.Type;
	if (activeCount != freeCount)
	{
		if (activeCount < freeCount)
		{
			shader->DrawIndexed(0, pass, (freeCount - activeCount) * 6, activeCount * 6);
		}
		else
		{
			shader->DrawIndexed(0, pass, (data.MaxParticles - activeCount) * 6, activeCount * 6);

			if(freeCount > 0)
				shader->DrawIndexed(0, pass, freeCount * 6);
		}
	}
}

void ParticleSystem::Add(Vector3 & position)
{
	if (Time::Get()->Running() - lastAddTime < 60.0f / 1000)
		return;

	lastAddTime = Time::Get()->Running();


	UINT nextFree = freeCount + 1;

	if (nextFree >= data.MaxParticles)
		nextFree = 0;

	if (nextFree == deactiveCount)
		return;


	Vector3 velocity = Vector3(1, 1, 1);
	velocity *= data.EmitterVelocitySensitivity;

	float horizontalAngle = Math::Random(0.0f, 1.0f) * Math::PI * 2.0f;
	float horizontalVelocity = Math::Lerp
	(
		data.MinHorizontalVelocity, data.MaxHorizontalVelocity, Math::Random(0.0f, 1.0f)
	);

	velocity.x += horizontalVelocity * cosf(horizontalAngle);
	velocity.y += horizontalVelocity * sinf(horizontalAngle);

	velocity.z += Math::Lerp
	(
		data.MinVerticalVelocity, data.MaxVerticalVelocity, Math::Random(0.0f, 1.0f)
	);

	Color color = Math::RandomColor4();
	for (int i = 0; i < 4; i++)
	{
		vertices[freeCount * 4 + i].Position = position;
		vertices[freeCount * 4 + i].Velocity = velocity;
		vertices[freeCount * 4 + i].Random = color;
		vertices[freeCount * 4 + i].Time = currentTime;
	}
	
	freeCount = nextFree;
}

void ParticleSystem::SetTexture(wstring file)
{
	SafeDelete(map);

	data.TextureFile = file;
	map = new Texture(L"Particles/" + file);
}

void ParticleSystem::ReadFile(wstring file)
{
	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile(String::ToString(file).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();

	Xml::XMLElement* node = root->FirstChildElement();
	data.Type = (ParticleData::BlendType)node->IntText();

	node = node->NextSiblingElement();
	wstring textureFile = String::ToWString(node->GetText());
	data.TextureFile = L"Particles/" + textureFile;
	map = new Texture(data.TextureFile);


	node = node->NextSiblingElement();
	data.MaxParticles = node->IntText();

	node = node->NextSiblingElement();
	data.Duration = node->FloatText();

	node = node->NextSiblingElement();
	data.DurationRandomness = node->FloatText();

	node = node->NextSiblingElement();
	data.EmitterVelocitySensitivity = node->FloatText();

	node = node->NextSiblingElement();
	data.MinHorizontalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxHorizontalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MinVerticalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxVerticalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.Gravity.x = node->FloatAttribute("X");
	data.Gravity.y = node->FloatAttribute("Y");
	data.Gravity.z = node->FloatAttribute("Z");

	node = node->NextSiblingElement();
	data.EndVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MinColor.r = node->FloatAttribute("R");
	data.MinColor.b = node->FloatAttribute("G");
	data.MinColor.g = node->FloatAttribute("B");
	data.MinColor.a = node->FloatAttribute("A");

	node = node->NextSiblingElement();
	data.MaxColor.r = node->FloatAttribute("R");
	data.MaxColor.b = node->FloatAttribute("G");
	data.MaxColor.g = node->FloatAttribute("B");
	data.MaxColor.a = node->FloatAttribute("A");

	node = node->NextSiblingElement();
	data.MinRotateSpeed = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxRotateSpeed = node->FloatText();

	node = node->NextSiblingElement();
	data.MinStartSize = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxStartSize = node->FloatText();

	node = node->NextSiblingElement();
	data.MinEndSize = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxEndSize = node->FloatText();

	SafeDelete(document);
}

void ParticleSystem::MapVertices()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	if (newCount < freeCount)
	{
		D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);
		{
			UINT offset = newCount * sizeof(VertexParticle) * 4;
			UINT start = newCount * 4;
			UINT size = (freeCount - newCount) * 4;

			BYTE* p = (BYTE *)subResource.pData + offset;
			memcpy(p, vertices + start, size * sizeof(VertexParticle));

		}
		D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
	}
	else
	{
		D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);
		{
			UINT offset = newCount * sizeof(VertexParticle) * 4;
			UINT start = newCount * 4;
			UINT size = (data.MaxParticles - newCount) * 4;

			BYTE* p = (BYTE *)subResource.pData + offset;
			memcpy(p, vertices + start, size * sizeof(VertexParticle));

		}
		D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);

		if (freeCount > 0)
		{
			UINT size = freeCount * 4;

			memcpy(subResource.pData, vertices, size * sizeof(VertexParticle));
		}
	}

	newCount = freeCount;
}

void ParticleSystem::Reset()
{
	currentTime = 0.0f;
	lastAddTime = Time::Get()->Running();
	activeCount = newCount = freeCount = deactiveCount = 0;

	
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);

	vertices = new VertexParticle[data.MaxParticles * 4];
	for (UINT i = 0; i < data.MaxParticles; i++)
	{
		vertices[i * 4 + 0].Corner = Vector2(-1, -1);
		vertices[i * 4 + 1].Corner = Vector2(+1, -1);
		vertices[i * 4 + 2].Corner = Vector2(+1, +1);
		vertices[i * 4 + 3].Corner = Vector2(-1, +1);
	}

	indices = new UINT[data.MaxParticles * 6];
	for (UINT i = 0; i < data.MaxParticles; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;

		indices[i * 6 + 3] = i * 4 + 0;
		indices[i * 6 + 4] = i * 4 + 2;
		indices[i * 6 + 5] = i * 4 + 3;
	}

	vertexBuffer = new VertexBuffer(vertices, data.MaxParticles * 4, sizeof(VertexParticle), 0, true);
	indexBuffer = new IndexBuffer(indices, data.MaxParticles * 6);
}

void ParticleSystem::Activate()
{
	while (activeCount != newCount)
	{
		float age = currentTime - vertices[activeCount * 4].Time;

		if (age < data.Duration)
			return;

		vertices[activeCount * 4].Time = currentTime;
		activeCount++;

		if (activeCount >= data.MaxParticles)
			activeCount = 0;
	}
}

void ParticleSystem::Deactivate()
{
	while (deactiveCount != activeCount)
	{
		float age = currentTime - vertices[deactiveCount * 4].Time;

		if (age > data.Duration)
			return;

		deactiveCount++;

		if (deactiveCount > data.MaxParticles)
			deactiveCount = 0;
	}
}


