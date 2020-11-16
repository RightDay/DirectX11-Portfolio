cbuffer CB_PostEffect
{
    float2 PixelSize;
};

#define MAX_POSTEFFECT_SRVS 8
Texture2D Maps[MAX_POSTEFFECT_SRVS];

struct VertexOutput_PostEffect
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

VertexOutput_PostEffect VS_PostEffect(float4 Position : Position)
{
    VertexOutput_PostEffect output;
    
    output.Position = Position;
    output.Uv.x = Position.x * 0.5f + 0.5f;
    output.Uv.y = -Position.y * 0.5f + 0.5f;
    
    return output;
}