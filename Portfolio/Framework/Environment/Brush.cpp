#include "Framework.h"
#include "Brush.h"

Brush::Brush(Shader* shader, Terrain* terrain)
	: shader(shader), terrain(terrain) 
{
	brushBuffer = new ConstantBuffer(&brushDesc, sizeof(BrushDesc));
	sBrushBuffer = shader->AsConstantBuffer("CB_TerrainBrush");
}

Brush::~Brush()
{
	SafeDelete(brushBuffer);
}

void Brush::Update()
{
	location = brushDesc.Location;
}

void Brush::Render()
{
	brushBuffer->Apply();
	sBrushBuffer->SetConstantBuffer(brushBuffer->Buffer());
}

void Brush::RaiseHeight(UINT type, UINT range)
{
	UINT left = (UINT)location.x - brushDesc.Range;
	UINT right = (UINT)location.x + brushDesc.Range;
	UINT bottom = (UINT)location.z - brushDesc.Range;
	UINT top = (UINT)location.z + brushDesc.Range;
}