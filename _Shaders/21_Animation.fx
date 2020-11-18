#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float4 PS(MeshOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    if(any(diffuse) == false)
        diffuse = float4(1, 1, 1, 1);
    
    float NdotL = dot(-GlobalLight.Direction, normalize(input.Normal));
    
    return (diffuse * NdotL);
}

technique11 T0
{
    P_VP(P0, VS_Mesh, PS)
    P_VP(P1, VS_Model, PS)
    P_VP(P2, VS_Animation, PS)
}