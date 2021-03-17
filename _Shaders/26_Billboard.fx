#include "00_Global.fx"
#include "00_Light.fx"

struct VertexInput
{
    float4 Position : Position;
    float2 Scale : Scale;

    uint VertexID : SV_VertexID;
};

struct VertexOutput
{
    float4 Position : Position;
    float2 Scale : Scale;

    uint VertexID : VertexID;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    //output.Position = WorldPosition(input.Position);
    //output.Scale = input.Scale;

    //output.Position = float4(input.VertexID * 10.0f, 0, 0, 1); // position billboard에서 받기
    output.Position = input.Position; // position billboard에서 받기
    output.Scale = input.Scale;

    output.VertexID = input.VertexID;

    return output;
}


struct GeometryOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;

    uint VertexID : VertexID;
};

[maxvertexcount(4)]
void GS(point VertexOutput input[1], inout TriangleStream<GeometryOutput> stream)
{
    float3 up = float3(0, 1, 0);
    //float3 forward = float3(0, 0, 1);
    float3 forward = input[0].Position.xyz - ViewPosition();
    float3 right = normalize(cross(up, forward));

    float2 size = input[0].Scale * 0.5f;

    float4 position[4];
    position[0] = float4(input[0].Position.xyz - size.x * right - size.y * up, 1);
    position[1] = float4(input[0].Position.xyz - size.x * right + size.y * up, 1);
    position[2] = float4(input[0].Position.xyz + size.x * right - size.y * up, 1);
    position[3] = float4(input[0].Position.xyz + size.x * right + size.y * up, 1);


    float2 uv[4] =
    {
        float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)
    };


    GeometryOutput output;

    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        output.Position = ViewProjection(position[i]);
        output.Uv = uv[i];
        output.VertexID = input[0].VertexID;

        stream.Append(output);
    }

}

Texture2DArray Maps;
float4 PS(GeometryOutput input) : SV_Target
{
    float4 diffuse = Maps.Sample(LinearSampler, float3(input.Uv, input.VertexID % 6));
    return diffuse;

    //return float4((float) input.VertexID / 100.0f, 0, 0, 1);
}

technique11 T0
{
    P_BS_VGP(P0, AlphaBlend_AlphaToCoverage, VS, GS, PS)
}