#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Terrain.fx"

float4 PS(VertexTerrain input) : SV_Target
{
    //float4 base = BaseMap.Sample(LinearSampler, input.Uv);
    //float4 layer = LayerMap.Sample(LinearSampler, input.Uv);
    //float4 color = lerp(base, layer, input.Uv.x);
    
    /*float4 color = GetLayerColor(input.Uv);
    float NdotL = dot(-GlobalLight.Direction, normalize(input.Normal));
    
    return (color * NdotL);*/

    float2 left = input.Uv + float2(-TexelCellSpaceU, 0.0f);
    float2 right = input.Uv + float2(+TexelCellSpaceU, 0.0f);
    float2 top = input.Uv + float2(0.0f, -TexelCellSpaceV);
    float2 bottom = input.Uv + float2(0.0f, TexelCellSpaceV);

    float leftY = HeightMap.SampleLevel(LinearSampler, left, 0).r * 255 / TerrainHeightRatio;
    float rightY = HeightMap.SampleLevel(LinearSampler, right, 0).r * 255 / TerrainHeightRatio;
    float topY = HeightMap.SampleLevel(LinearSampler, top, 0).r * 255 / TerrainHeightRatio;
    float bottomY = HeightMap.SampleLevel(LinearSampler, bottom, 0).r * 255 / TerrainHeightRatio;

    float3 tangent = normalize(float3(WorldCellSpace * 2.0f, rightY - leftY, 0.0f));
    float3 biTangent = normalize(float3(0.0f, bottomY - topY, WorldCellSpace * -2.0f));
    float3 normal = normalize(cross(tangent, biTangent));

    float4 brush = GetBrushColor(input.wPosition);

    Material.Diffuse = GetLayerColor(input.Uv);
    NormalMapping(input.Uv, normal, tangent);

    return Material.Diffuse;
}

float4 PS_GridLine(VertexTerrain input) : SV_Target
{
    //float4 base = BaseMap.Sample(LinearSampler, input.Uv);
    //float4 layer = LayerMap.Sample(LinearSampler, input.Uv);
    //float4 color = lerp(base, layer, input.Uv.x);
    
    float4 color = GetLayerColor(input.Uv);
    
    float NdotL = dot(-GlobalLight.Direction, normalize(input.Normal));
    
    float4 brushColor = GetBrushColor(input.wPosition);
    float4 lineColor = GetLineColor(input.wPosition);
    
    return (color * NdotL) + brushColor + lineColor;
    //return lineColor;
}

technique11 T0
{
    P_VP(P0, VS_Terrain, PS)
    P_RS_VP(P1, FillMode_WireFrame, VS_Terrain, PS)
    P_VP(P2, VS_Terrain, PS_GridLine)
}