#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Terrain.fx"

float4 PS(VertexTerrain input) : SV_Target
{
    float4 base = BaseMap.Sample(LinearSampler, input.Uv);
    float NdotL = dot(-GlobalLight.Direction, normalize(input.Normal));
    
    float4 brush = GetBrushColor(input.wPosition);
    
    return (base * NdotL) + brush;
}

technique11 T0
{
    P_VP(P0, VS_Terrain, PS)
    P_RS_VP(P1, FillMode_WireFrame, VS_Terrain, PS)
}