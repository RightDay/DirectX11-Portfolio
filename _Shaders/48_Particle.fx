#include "00_Global.fx"

Texture2D ParticleMap;

struct ParticleDesc
{
    float4 MinColor;
    float4 MaxColor;
    
    float3 Gravity;
    float EndVelocity;
    
    float2 StartSize;
    float2 EndSize;
    
    float2 RotateSpeed;
    float Duration;
    float DurationRandomness;
    
    float CurrentTime;
};

cbuffer CB_Particle
{
    ParticleDesc Particle;
};

struct VertexInput
{
    float4 Position : Position;
    float2 Corner : Uv;
    float3 Velocity : Velocity;
    float4 Random : Random; //x : 주기, y - 크기, z - 색상, w - 회전
    float Time : Time;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float4 Color : Color;
    float2 Uv : Uv;
};

float4 ComputePosition(float3 position, float3 velocity, float age, float normalizedAge)
{
    float startVelocity = length(velocity);
    float endVelocity = startVelocity * Particle.EndVelocity;
    
    float velocityIntegral = startVelocity * normalizedAge + (endVelocity - startVelocity) * normalizedAge / 2;

    position += normalize(velocity) * velocityIntegral * Particle.Duration;
    position += Particle.Gravity * age * normalizedAge;
    
    return ViewProjection(float4(position, 1));
}

float ComputeSize(float randomValue, float normalizedAge)
{
    float startSize = lerp(Particle.StartSize.x, Particle.StartSize.y, randomValue);
    float endSize = lerp(Particle.EndSize.x, Particle.EndSize.y, randomValue);
    
    return lerp(startSize, endSize, normalizedAge);
}

float2x2 ComputeRotation(float randomValue, float age)
{
    float rotationSpeed = lerp(Particle.RotateSpeed.x, Particle.RotateSpeed.y, randomValue);
    float rotation = rotationSpeed * age;
    
    float c = cos(rotation);
    float s = sin(rotation);
    
    return float2x2(c, -s, s, c);
}

float4 ComputeColor(float4 projectedPosition, float randomValue, float normalizedAge)
{
    float4 color = lerp(Particle.MinColor, Particle.MaxColor, randomValue);
    color.a *= normalizedAge * (1 - normalizedAge) * (1 - normalizedAge) * 6.7f;
    
    return color;
}

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    float age = Particle.CurrentTime - input.Time;
    age *= input.Random.x * Particle.DurationRandomness + 1;
    
    float normalizedAge = saturate(age / Particle.Duration);
    
    output.Position = ComputePosition(input.Position.xyz, input.Velocity, age, normalizedAge);
    
    float size = ComputeSize(input.Random.y, normalizedAge);
    float2x2 rotation = ComputeRotation(input.Random.w, age);
    
    output.Position.xy += mul(input.Corner, rotation) * size * float2(0.5f, -0.5f);
    
    output.Uv = (input.Corner + 1.0f) * 0.5f;
    output.Color = ComputeColor(output.Position, input.Random.z, normalizedAge);
    
    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    return ParticleMap.Sample(LinearSampler, input.Uv) * input.Color;
}


BlendState OpaqueBlend_Particle
{
    BlendEnable[0] = true;
    DestBlend[0] = Zero;
    SrcBlend[0] = One;
    BlendOp[0] = Add;

    DestBlendAlpha[0] = Zero;
    SrcBlendAlpha[0] = One;
    BlendOpAlpha[0] = Add;
    
    RenderTargetWriteMask[0] = 15;
};

BlendState AdditiveBlend_Particle
{
    BlendEnable[0] = true;
    DestBlend[0] = One;
    SrcBlend[0] = SRC_ALPHA;
    BlendOp[0] = Add;

    DestBlendAlpha[0] = One;
    SrcBlendAlpha[0] = SRC_ALPHA;
    BlendOpAlpha[0] = Add;
    
    RenderTargetWriteMask[0] = 15;
};

BlendState AlphaBlend_Particle
{
    BlendEnable[0] = true;
    DestBlend[0] = INV_SRC_ALPHA;
    SrcBlend[0] = SRC_ALPHA;
    BlendOp[0] = Add;

    SrcBlendAlpha[0] = Zero;
    DestBlendAlpha[0] = Zero;
    BlendOpAlpha[0] = Add;
    
    RenderTargetWriteMask[0] = 15;
};

DepthStencilState DepthRead
{
    DepthEnable = true;
    DepthWriteMask = 0;
    DepthFunc = Always;
    StencilEnable = false;
};

technique11 T0
{
    P_DSS_BS_VP(P0, DepthRead, OpaqueBlend_Particle, VS, PS)
    P_DSS_BS_VP(P1, DepthRead, AdditiveBlend_Particle, VS, PS)
    P_DSS_BS_VP(P2, DepthRead, AlphaBlend_Particle, VS, PS)
}