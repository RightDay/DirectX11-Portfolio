#pragma once
#include "Systems/IExecute.h"

class Editor : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override {}

private:
	void Mesh();
	void OnGUI();
	void OnGUI_List();
	void OnGUI_Write();
	void OnGUI_Setttings();

	void UpdateParticleList();
	void UpdateTextureList();

	void WriteFile(wstring file);

private:
	wstring file = L"";
	ParticleSystem* particle = NULL;

	UINT maxParticle = 0;

	vector<wstring> particleList;
	vector<wstring> textureList;

	Shader* shader;
	Shadow* shadow;
	
	CubeSky* sky;

	Material* floor;
	Material* brick;

	MeshRender* grid;
	MeshRender* sphere;
};