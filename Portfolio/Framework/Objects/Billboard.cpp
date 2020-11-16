#include "Framework.h"
#include "Billboard.h"

Billboard::Billboard(Shader * shader)
	: Renderer(shader)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	sMaps = shader->AsSRV("Maps");
}

Billboard::~Billboard()
{
	SafeDelete(textures);
}

void Billboard::Add(Vector3 & position, Vector2 & scale)
{
	VertexScale vertex;
	vertex.Position = position;
	vertex.Scale = scale;

	vertices.push_back(vertex);
}

void Billboard::AddTexture(wstring file)
{
	SafeDelete(textures);

	textureFiles.push_back(file);
}

void Billboard::Update()
{
	Super::Update();
}

void Billboard::Render()
{
	if (vertices.size() != vertexCount)
	{
		vertexCount = vertices.size();

		SafeDelete(vertexBuffer);
		vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(VertexScale));
	}

	if (textureFiles.size() > 0 && textures == NULL)
		textures = new TextureArray(textureFiles);


	Super::Render();

	sMaps->SetResource(textures->SRV());
	shader->Draw(0, Pass(), vertexCount);
}
