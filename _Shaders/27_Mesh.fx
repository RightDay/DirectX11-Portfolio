#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float4 PS(MeshOutput input) : SV_Target
{
    float3 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    float NdotL = dot(normalize(input.Normal), -GlobalLight.Direction);

    return float4(diffuse * NdotL, 1);
}

SamplerState Sampler;

SamplerState Sampler_Border
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Border;
    AddressV = Border;

    BorderColor = float4(0, 0, 0, 1);
};
float4 PS_1(MeshOutput input) : SV_Target
{
    float3 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);

    return float4(diffuse, 1);
}

technique11 T0
{
    P_VP(P0, VS_Mesh, PS)
    P_VP(P1, VS_Mesh, PS_1)
}