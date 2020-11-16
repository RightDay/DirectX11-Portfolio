cbuffer CB_Water
{
    float4 RefractionColor;

    float2 NormalMapTile;
    float WaveTranslation = 0.0f;
    float WaveScale = 0.05f;

    float WaterShininess = 30.0f;
    float WaterAlpha = 0.5f;
    float WaveSpeed = 0.06f;
};

struct VertexOutput_Water
{
    float4 Position : SV_Position;
    float3 wPosition : Position1;
    float4 ReflectionPosition : Position2;
    float4 RefractionPosition : Position3;
    
    float2 Uv : Uv;
    float2 Uv2 : Uv1;
};

VertexOutput_Water VS_Water(VertexTexture input)
{
    VertexOutput_Water output;

    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;
    
    output.Position = ViewProjection(output.Position); //WVP
    
    
    //W Reflection P
    output.ReflectionPosition = WorldPosition(input.Position);
    output.ReflectionPosition = mul(output.ReflectionPosition, Reflection);
    output.ReflectionPosition = mul(output.ReflectionPosition, Projection);
    
    output.RefractionPosition = output.Position; //WVP
    
    
    output.Uv = input.Uv / NormalMapTile.x;
    output.Uv2 = input.Uv / NormalMapTile.y;
    
    return output;
}

float4 PS_Water(VertexOutput_Water input) : SV_Target
{
    input.Uv.y += WaveTranslation;
    input.Uv2.x += WaveTranslation;
    
    float4 normalMap = NormalMap.Sample(LinearSampler, input.Uv) * 2.0f - 1.0f;
    float4 normalMap2 = NormalMap.Sample(LinearSampler, input.Uv2) * 2.0f - 1.0f;
    float3 normal = normalMap.rgb + normalMap2.rgb;
    
    float2 reflection;
    reflection.x = input.ReflectionPosition.x / input.ReflectionPosition.w * 0.5f + 0.5f;
    reflection.y = -input.ReflectionPosition.y / input.ReflectionPosition.w * 0.5f + 0.5f;
    reflection += normal.xy * WaveScale;
    float4 reflectionColor = ReflectionMap.Sample(LinearSampler, reflection);
    
    float2 refraction;
    refraction.x = input.RefractionPosition.x / input.RefractionPosition.w * 0.5f + 0.5f;
    refraction.y = -input.RefractionPosition.y / input.RefractionPosition.w * 0.5f + 0.5f;
    refraction += normal.xy * WaveScale;
    float4 refractionColor = saturate(RefractionMap.Sample(LinearSampler, refraction) + RefractionColor);
    
    
    float3 light = GlobalLight.Direction;
    light.y *= -1.0f;
    light.z *= -1.0f;
    
    float3 view = normalize(ViewPosition() - input.wPosition);
    float3 heightView = view.yyy;
    
    float r = (1.2f - 1.0f) / (1.2f / 1.0f);
    float fresnel = saturate(min(1, r + (1 - r) * pow(1 - dot(normal, heightView), 2)));
    float4 diffuse = lerp(reflectionColor, refractionColor, fresnel);
    
    
    float3 R = normalize(reflect(light, normal));
    float specular = saturate(dot(R, view));
    
    [flatten]
    if (specular > 0.0f)
    {
        specular = pow(specular, WaterShininess);
        diffuse = saturate(diffuse + specular);
    }
    
    return float4(diffuse.rgb, WaterAlpha);
}