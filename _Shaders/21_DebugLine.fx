#include "00_Global.fx"
#include "00_Light.fx"

struct VertexOutput
{
    float4 Position : SV_Position0;
    float4 Color : Color0;
};

VertexOutput VS(VertexColor input)
{
    VertexOutput output;
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    output.Color = input.Color;
    
    return output;
}

float4 PS(VertexOutput input) : SV_Target0
{
    return input.Color;
}

technique11 T0
{
    P_VP(P0, VS, PS)
}