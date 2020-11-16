Texture2D BaseMap;
Texture2D AlphaMap;
Texture2D LayerMap;
Texture2D HeightMap;

struct VertexTerrain
{
    float4 Position : SV_Position;
    float3 wPosition : Position1;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

VertexTerrain VS_Terrain(VertexTextureNormal input)
{
    VertexTerrain output;
    output.Position = mul(input.Position, World);
    output.wPosition = output.Position.xyz;
    
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.Normal = WorldNormal(input.Normal);
    output.Uv = input.Uv;
    
    return output;
}

///////////////////////////////////////////////////////////////////////////////

float4 GetLayerColor(SamplerState samp, float2 uv)
{
    float4 base = BaseMap.Sample(samp, uv);
    float4 layer = LayerMap.Sample(samp, uv);
    float alpha = AlphaMap.Sample(samp, uv).r;
    
    return lerp(base, layer, (1 - alpha) * 0.4f);
}

float4 GetLayerColor(float2 uv)
{
    return GetLayerColor(LinearSampler, uv);
}

///////////////////////////////////////////////////////////////////////////////

struct BrushDesc
{
    float4 Color;
    float3 Location;
    uint Type;
    uint Range;
};

cbuffer CB_Brush
{
    BrushDesc TerrainBrush;
};

float4 GetBrushColor(float3 wPosition)
{
    [flatten]
    if(TerrainBrush.Type == 1)
    {
        [flatten]
        if((wPosition.x >= (TerrainBrush.Location.x - TerrainBrush.Range)) &&
            (wPosition.x <= (TerrainBrush.Location.x + TerrainBrush.Range)) &&
            (wPosition.z >= (TerrainBrush.Location.z - TerrainBrush.Range)) &&
            (wPosition.z <= (TerrainBrush.Location.z + TerrainBrush.Range)))
        {
            return TerrainBrush.Color;
        }
    }
    
    [flatten]
    if(TerrainBrush.Type == 2)
    {
        float dx = wPosition.x - TerrainBrush.Location.x;
        float dz = wPosition.z - TerrainBrush.Location.z;
        float dist = sqrt(dx * dx + dz * dz);
        
        [flatten]
        if(dist <= (float)TerrainBrush.Range)
            return TerrainBrush.Color;
    }
    
    return float4(0, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////

struct LineDesc
{
    float4 Color;
    uint Visible;
    float Thickness;
    float Size;
};

cbuffer CB_TerrainLine
{
    LineDesc TerrainLine;
};

float4 GetBasicLineColor(float3 wPosition)
{
    [flatten]
    if(TerrainLine.Visible < 1)
        return float4(0, 0, 0, 0);
    
    float2 grid = wPosition.xz / TerrainLine.Size;
    grid = frac(grid);
    
    float thick = TerrainLine.Thickness / TerrainLine.Size;
    
    [flatten]
    if (grid.x < thick || grid.y < thick)
        return TerrainLine.Color;
    
    return float4(0, 0, 0, 0);
}

float4 GetLineColor(float3 wPosition)
{
    [flatten]
    if (TerrainLine.Visible < 1)
        return float4(0, 0, 0, 0);
    
    //float2 grid = wPosition.xz;
    float2 grid = wPosition.xz / TerrainLine.Size;
    float2 range = abs(frac(grid - 0.5f) - 0.5f);
    //return float4(range, 0, 1);
    
    float2 speed = fwidth(grid);
    //return float4(speed, 0, 1);
    
    float2 pixel = range / speed;
    //return float4(pixel, 0, 1);
    
    float thick = saturate(min(pixel.x, pixel.y) - TerrainLine.Thickness);
    //return float4(pixel, 0, 1);
    
    return lerp(TerrainLine.Color, float4(0, 0, 0, 0), thick);
}


///////////////////////////////////////////////////////////////////////////////
// TerrainLod
///////////////////////////////////////////////////////////////////////////////
cbuffer CB_TerrainLod
{    
    float MinDistance;
    float MaxDistance;
    float MinTessellation;
    float MaxTessellation;

    float TexelCellSpaceU;
    float TexelCellSpaceV;
    float WorldCellSpace;
    float TerrainHeightRatio;    
    
    float4 WorldFrustumPlanes[6];
};

struct VertexInput_TerrainLod
{
    float4 Position : Position;
    float2 Uv : Uv;
    float2 BoundsY : Bound;
};

struct VertexOutput_TerrainLod
{
    float4 Position : Position;
    float2 Uv : Uv;
    float2 BoundsY : Bound;
};

VertexOutput_TerrainLod VS_TerrainLod(VertexInput_TerrainLod input)
{
    VertexOutput_TerrainLod output;

    output.Position = input.Position;
    output.Uv = input.Uv;
    output.BoundsY = input.BoundsY;

    return output;
}


struct CHullOutput_TerrainLod
{
    float Edge[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

float TessellationFactor(float3 position)
{
    position = float3(position.x, 0.0f, position.z);
    float3 view = float3(ViewPosition().x, 0.0f, ViewPosition().z);

    float d = distance(position, view);
    float s = saturate((d - MaxDistance) / (MinDistance - MaxDistance));

    return lerp(MinTessellation, MaxTessellation, s);
}

bool PlaneTest(float3 center, float3 extents, float4 plane)
{
    float3 n = abs(plane.xyz);
    float r = dot(extents, n);
    float s = dot(float4(center, 1), plane);
    
    return (s + r) < 0.0f;
}

bool InFrustumCube(float3 center, float3 extents)
{
    [unroll(6)]
    for (int i = 0; i < 6; i++)
    {
        [flatten]
        if(PlaneTest(center, extents, WorldFrustumPlanes[i]))
            return true;
    }
    
    return false;
}

CHullOutput_TerrainLod
    CHS_TerrainLod(
    InputPatch<VertexOutput_TerrainLod, 4> input)
{
    float minY = input[0].BoundsY.x;
    float maxY = input[0].BoundsY.y;
    
    float3 vMin = float3(input[2].Position.x, minY, input[2].Position.z);
    float3 vMax = float3(input[1].Position.x, maxY, input[1].Position.z);
    
    float3 boxCenter = (vMin + vMax) * 0.5f;
    float3 boxExtents = (vMax - vMin) * 0.5f;    
    
    
    CHullOutput_TerrainLod output;
    
    [flatten]
    if(InFrustumCube(boxCenter, boxExtents))
    {
        output.Edge[0] = output.Edge[1] = output.Edge[2] = output.Edge[3] = -1;
        output.Inside[0] = output.Inside[1] = -1;

        return output;
    }
    

    float3 e0 = (input[0].Position + input[2].Position).xyz * 0.5f;
    float3 e1 = (input[0].Position + input[1].Position).xyz * 0.5f;
    float3 e2 = (input[1].Position + input[3].Position).xyz * 0.5f;
    float3 e3 = (input[2].Position + input[3].Position).xyz * 0.5f;
    
    output.Edge[0] = TessellationFactor(e0);
    output.Edge[1] = TessellationFactor(e1);
    output.Edge[2] = TessellationFactor(e2);
    output.Edge[3] = TessellationFactor(e3);
    
    float3 c = (input[0].Position + input[1].Position + input[2].Position + input[3].Position).xyz * 0.25f;
    output.Inside[0] = TessellationFactor(c);
    output.Inside[1] = TessellationFactor(c);
    
    return output;
}


struct HullOutput_TerrainLod
{
    float4 Position : Position;
    float2 Uv : Uv;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
//[partitioning("fractional_even")]
[outputcontrolpoints(4)]
[patchconstantfunc("CHS_TerrainLod")]
HullOutput_TerrainLod HS_TerrainLod(InputPatch<VertexOutput_TerrainLod, 4> input, uint id : SV_OutputControlPointID)
{
    HullOutput_TerrainLod output;
    
    output.Position = input[id].Position;
    output.Uv = input[id].Uv;

    return output;
}

struct DomainOutput_TerrainLod
{
    float4 Position : SV_Position;
    float3 wPosition : Position1;
    float2 Uv : Uv;
};

[domain("quad")]
DomainOutput_TerrainLod DS_TerrainLod(CHullOutput_TerrainLod input, OutputPatch<HullOutput_TerrainLod, 4> patch, float2 uv : SV_DomainLocation)
{
    DomainOutput_TerrainLod output;

    float3 p0 = lerp(patch[0].Position, patch[1].Position, uv.x).xyz;
    float3 p1 = lerp(patch[2].Position, patch[3].Position, uv.x).xyz;
    float3 position = lerp(p0, p1, uv.y);
    
    float2 uv0 = lerp(patch[0].Uv, patch[1].Uv, uv.x);
    float2 uv1 = lerp(patch[2].Uv, patch[3].Uv, uv.x);
    output.Uv = lerp(uv0, uv1, uv.y);
    
    position.y = HeightMap.SampleLevel(LinearSampler, output.Uv, 0).r * 255 / TerrainHeightRatio;
    
    output.Position = WorldPosition(float4(position, 1));
    output.wPosition = output.Position.xyz;
    
    output.Position = ViewProjection(output.Position);
    
    return output;
}

float4 PS_TerrainLod(DomainOutput_TerrainLod input) : SV_Target
{
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