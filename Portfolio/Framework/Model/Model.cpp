#include "Framework.h"
#include "Model.h"
#include "Utilities/Xml.h"
#include "Utilities/BinaryFile.h"

Model::Model()
{
}

Model::~Model()
{
	for (Material* material : materials)
		SafeDelete(material);

	for (ModelBone* bone : bones)
		SafeDelete(bone);

	for (ModelMesh* mesh : meshes)
		SafeDelete(mesh);

	for (ModelClip* clip : clips)
		SafeDelete(clip);
}

Material* Model::MaterialByName(wstring name)
{
	for (Material* material : materials)
	{
		if (material->Name() == name)
			return material;
	}

	return NULL;
}

ModelBone* Model::BoneByName(wstring name)
{
	for (ModelBone* bone : bones)
	{
		if (bone->Name() == name)
			return bone;
	}

	return NULL;
}

ModelMesh* Model::MeshByName(wstring name)
{
	for (ModelMesh* mesh : meshes)
	{
		if (mesh->Name() == name)
			return mesh;
	}

	return NULL;
}

ModelClip* Model::ClipByName(wstring name)
{
	for (ModelClip* clip : clips)
	{
		if (clip->name == name)
			return clip;
	}

	return NULL;
}

void Model::Attach(Shader* shader, Model* model, int parentBoneIndex, Transform* transform)
{
	//Copy Material
	for (Material* material : model->Materials())
	{
		Material* newMaterial = new Material(shader);

		newMaterial->Name(material->Name());
		newMaterial->Ambient(material->Ambient());
		newMaterial->Diffuse(material->Diffuse());
		newMaterial->Specular(material->Specular());
		newMaterial->Emissive(material->Emissive());

		if (material->DiffuseMap() != NULL)
			newMaterial->DiffuseMap(material->DiffuseMap()->GetFile());

		if (material->SpecularMap() != NULL)
			newMaterial->SpecularMap(material->SpecularMap()->GetFile());

		if (material->NormalMap() != NULL)
			newMaterial->NormalMap(material->NormalMap()->GetFile());

		materials.push_back(newMaterial);
	}

	vector<pair<int, int>> changes;
	//Copy Bone
	{
		ModelBone* parentBone = BoneByIndex(parentBoneIndex);

		for (ModelBone* bone : model->Bones())
		{
			ModelBone* newBone = new ModelBone;
			newBone->name = bone->name;
			newBone->transform = bone->transform;

			if (transform != NULL)
				newBone->transform = newBone->transform * transform->World();

			if (bone->parent != NULL)
			{
				int parentIndex = bone->parentIndex;

				for (pair<int, int>& temp : changes)
				{
					if (temp.first == parentIndex)
					{
						newBone->parentIndex = temp.second;
						newBone->parent = bones[newBone->parentIndex];
						newBone->parent->childs.push_back(newBone);

						break;
					}
				}//for(pair)
			}
			else
			{
				newBone->parentIndex = parentBoneIndex;
				newBone->parent = parentBone;
				newBone->parent->childs.push_back(newBone);
			}

			newBone->index = bones.size();
			changes.push_back(pair<int, int>(bone->index, newBone->index));

			bones.push_back(newBone);
		}//for(bone)
	}

	//Copy Mesh
	{
		for (ModelMesh* mesh : model->Meshes())
		{
			ModelMesh* newMesh = new ModelMesh();
			for (pair<int, int>& temp : changes)
			{
				if (temp.first == mesh->boneIndex)
				{
					newMesh->boneIndex = temp.second;

					break;
				}
			}


			newMesh->bone = bones[newMesh->boneIndex];
			newMesh->name = mesh->name;
			newMesh->materialName = mesh->materialName;

			newMesh->vertexCount = mesh->vertexCount;
			newMesh->indexCount = mesh->indexCount;

			UINT verticesSize = newMesh->vertexCount * sizeof(ModelVertex);
			newMesh->vertices = new ModelVertex[newMesh->vertexCount];
			memcpy_s(newMesh->vertices, verticesSize, mesh->vertices, verticesSize);

			UINT indicesSize = newMesh->indexCount * sizeof(UINT);
			newMesh->indices = new UINT[newMesh->indexCount];
			memcpy_s(newMesh->indices, indicesSize, mesh->indices, indicesSize);


			newMesh->Binding(this);
			newMesh->SetShader(shader);

			meshes.push_back(newMesh);
		}
	}
}


void Model::ReadMaterial(wstring file)
{
	file = L"../../_Textures/" + file + L".material";

	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile(String::ToString(file).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();
	Xml::XMLElement* materialNode = root->FirstChildElement();

	do
	{
		Material* material = new Material();


		Xml::XMLElement* node = NULL;

		node = materialNode->FirstChildElement();
		material->Name(String::ToWString(node->GetText()));

		wstring directory = Path::GetDirectoryName(file);
		String::Replace(&directory, L"../../_Textures", L"");


		wstring texture = L"";

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->DiffuseMap(directory + texture);

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->SpecularMap(directory + texture);

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->NormalMap(directory + texture);


		D3DXCOLOR color;

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Ambient(color);

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Diffuse(color);

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Specular(color);

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Emissive(color);

		materials.push_back(material);

		materialNode = materialNode->NextSiblingElement();
	} while (materialNode != NULL);
}

void Model::ReadMesh(wstring file)
{
	file = L"../../_Models/" + file + L".mesh";


	BinaryReader* r = new BinaryReader();
	r->Open(file);

	UINT count = 0;

	count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		ModelBone* bone = new ModelBone();

		bone->index = r->Int();
		bone->name = String::ToWString(r->String());
		bone->parentIndex = r->Int();
		bone->transform = r->Matrix();

		bones.push_back(bone);
	}


	count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = new ModelMesh();

		mesh->name = String::ToWString(r->String());
		mesh->boneIndex = r->Int();

		mesh->materialName = String::ToWString(r->String());

		//VertexData
		{
			UINT count = r->UInt();

			vector<Model::ModelVertex> vertices;
			vertices.assign(count, Model::ModelVertex());

			void* ptr = (void*)&(vertices[0]);
			r->Byte(&ptr, sizeof(Model::ModelVertex) * count);


			mesh->vertices = new Model::ModelVertex[count];
			mesh->vertexCount = count;
			copy
			(
				vertices.begin(), vertices.end(),
				stdext::checked_array_iterator<Model::ModelVertex*>(mesh->vertices, count)
			);
		}

		//IndexData
		{
			UINT count = r->UInt();

			vector<UINT> indices;
			indices.assign(count, UINT());

			void* ptr = (void*)&(indices[0]);
			r->Byte(&ptr, sizeof(UINT) * count);


			mesh->indices = new UINT[count];
			mesh->indexCount = count;
			copy
			(
				indices.begin(), indices.end(),
				stdext::checked_array_iterator<UINT*>(mesh->indices, count)
			);
		}

		meshes.push_back(mesh);
	}//for(i)

	r->Close();
	SafeDelete(r);


	BindBone();
	BindMesh();
}

void Model::BindBone()
{
	root = bones[0];
	for (ModelBone* bone : bones)
	{
		if (bone->parentIndex > -1)
		{
			bone->parent = bones[bone->parentIndex];
			bone->parent->childs.push_back(bone);
		}
		else
			bone->parent = NULL;
	}
}

void Model::BindMesh()
{
	for (ModelMesh* mesh : meshes)
	{
		mesh->bone = bones[mesh->boneIndex];

		mesh->Binding(this);
	}
}

void Model::ReadClip(wstring file)
{
	file = L"../../_Models/" + file + L".clip";

	BinaryReader* r = new BinaryReader();
	r->Open(file);


	ModelClip* clip = new ModelClip();

	clip->name = String::ToWString(r->String());
	clip->duration = r->Float();
	clip->frameRate = r->Float();
	clip->frameCount = r->UInt();

	UINT keyframesCount = r->UInt();
	for (UINT i = 0; i < keyframesCount; i++)
	{
		ModelKeyframe* keyframe = new ModelKeyframe();
		keyframe->BoneName = String::ToWString(r->String());


		UINT size = r->UInt();
		if (size > 0)
		{
			keyframe->Transforms.assign(size, ModelKeyframeData());

			void* ptr = (void*)&keyframe->Transforms[0];
			r->Byte(&ptr, sizeof(ModelKeyframeData) * size);
		}

		clip->keyframeMap[keyframe->BoneName] = keyframe;
	}

	r->Close();
	SafeDelete(r);

	clips.push_back(clip);
}