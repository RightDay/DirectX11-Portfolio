struct VertexMesh
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    
    matrix Transform : InstTransform;
};

///////////////////////////////////////////////////////////////////////////////

struct DepthOutput
{
    float4 Position : SV_Position;
    float4 sPosition : Position1;
};

float4 PS_Depth(DepthOutput input) : SV_Target
{
    float depth = input.Position.z / input.Position.w;
    
    //depth = 1.0f - depth * 5.0f; //카메라 영역
    return float4(depth, depth, depth, 1.0f);
}

///////////////////////////////////////////////////////////////////////////////

#define VS_GENERATE \
output.oPosition = input.Position.xyz; \
output.Position = WorldPosition(input.Position); \
output.wPosition = output.Position.xyz; \
output.gPosition = output.Position; \
\
output.Position = ViewProjection(output.Position); \
output.wvpPosition = output.Position; \
output.wvpPosition_Sub = output.Position; \
\
output.sPosition = WorldPosition(input.Position);\
output.sPosition = mul(output.sPosition, ShadowView);\
output.sPosition = mul(output.sPosition, ShadowProjection);\
\
output.Normal = WorldNormal(input.Normal); \
output.Tangent = WorldTangent(input.Tangent); \
output.Uv = input.Uv; \
\
output.Culling.x = mul(float4(output.wPosition, 1), Culling[0]); \
output.Culling.y = mul(float4(output.wPosition, 1), Culling[1]); \
output.Culling.z = mul(float4(output.wPosition, 1), Culling[2]); \
output.Culling.w = mul(float4(output.wPosition, 1), Culling[3]); \
\
output.Clipping.x = mul(float4(output.wPosition, 1), Clipping); \
output.Clipping.y = 0.0f; \
output.Clipping.z = 0.0f; \
output.Clipping.w = 0.0f;


///////////////////////////////////////////////////////////////////////////////

//메인카메라 영역
//#define VS_DEPTH_GENERATE \
//output.Position = WorldPosition(input.Position); \
//output.Position = mul(output.Position, View); \
//output.Position = mul(output.Position, Projection); \
//\
//output.sPosition = output.Position;

//조명 영역
#define VS_DEPTH_GENERATE \
output.Position = WorldPosition(input.Position); \
output.Position = mul(output.Position, ShadowView); \
output.Position = mul(output.Position, ShadowProjection); \
\
output.sPosition = output.Position;

///////////////////////////////////////////////////////////////////////////////

void SetMeshWorld(inout matrix world, VertexMesh input)
{
    world = input.Transform;
}

MeshOutput VS_Mesh(VertexMesh input)
{
    MeshOutput output;
    
    SetMeshWorld(World, input);
    VS_GENERATE
    
    return output;
}

MeshOutput VS_PreRender_Reflection_Mesh(VertexMesh input)
{
    MeshOutput output = VS_Mesh(input);
    
    output.Position = WorldPosition(input.Position);
    output.Position = mul(output.Position, Reflection);
    output.Position = mul(output.Position, Projection);
    
    return output;
}

DepthOutput VS_Depth_Mesh(VertexMesh input)
{
    DepthOutput output;
    
    SetMeshWorld(World, input);
    VS_DEPTH_GENERATE
    
    return output;
}

///////////////////////////////////////////////////////////////////////////////

struct VertexModel
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 BlendIndices : BlendIndices;
    float4 BlendWeights : BlendWeights;
    
    matrix Transform : InstTransform;
    uint InstanceID : SV_InstanceID;
};

Texture2DArray TransformsMap;
#define MAX_MODEL_TRANSFORMS 250

cbuffer CB_Bone
{
    //matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
    
    uint BoneIndex;
};

void SetModelWorld(inout matrix world, VertexModel input)
{
    //world = mul(BoneTransforms[BoneIndex], world);
    
    float4 m0 = TransformsMap.Load(int4(BoneIndex * 4 + 0, input.InstanceID, 0, 0));
    float4 m1 = TransformsMap.Load(int4(BoneIndex * 4 + 1, input.InstanceID, 0, 0));
    float4 m2 = TransformsMap.Load(int4(BoneIndex * 4 + 2, input.InstanceID, 0, 0));
    float4 m3 = TransformsMap.Load(int4(BoneIndex * 4 + 3, input.InstanceID, 0, 0));

    matrix transform = matrix(m0, m1, m2, m3);
    world = mul(transform, input.Transform);
}

MeshOutput VS_Model(VertexModel input)
{    
    MeshOutput output;
    SetModelWorld(World, input);
    
    VS_GENERATE

    return output;
}

MeshOutput VS_PreRender_Reflection_Model(VertexModel input)
{
    MeshOutput output = VS_Model(input);
    
    output.Position = WorldPosition(input.Position);
    output.Position = mul(output.Position, Reflection);
    output.Position = mul(output.Position, Projection);
    
    return output;
}

DepthOutput VS_Depth_Model(VertexModel input)
{
    DepthOutput output;
    
    SetModelWorld(World, input);
    VS_DEPTH_GENERATE
    
    return output;
}

///////////////////////////////////////////////////////////////////////////////

#define MAX_MODEL_KEYFRAMES 500
#define MAX_MODEL_INSTANCE 500
struct AnimationFrame
{
    int Clip;

    uint CurrFrame;
    uint NextFrame;

    float Time;
    float Running;

    float3 Padding;
};

struct TweenFrame
{
    float TakeTime;
    float TweenTime;
    float RunningTime;
    float Padding;

    AnimationFrame Curr;
    AnimationFrame Next;
};

cbuffer CB_AnimationFrame
{
    TweenFrame TweenFrames[MAX_MODEL_INSTANCE];
};

void SetAnimationWorld(inout matrix world, VertexModel input)
{
    matrix transform = 0;
    matrix curr = 0, currAnim = 0;
    matrix next = 0, nextAnim = 0;
    

    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };
    
    //Attach Model
    [flatten]
    if (any(input.BlendIndices) == false)
    {
        indices[0] = BoneIndex;
        weights[0] = 1.0f;
    }
        
    
    uint clip[2];
    uint currFrame[2];
    uint nextFrame[2];
    float time[2];
    
    clip[0] = TweenFrames[input.InstanceID].Curr.Clip;
    currFrame[0] = TweenFrames[input.InstanceID].Curr.CurrFrame;
    nextFrame[0] = TweenFrames[input.InstanceID].Curr.NextFrame;
    time[0] = TweenFrames[input.InstanceID].Curr.Time;
    
    clip[1] = TweenFrames[input.InstanceID].Next.Clip;
    currFrame[1] = TweenFrames[input.InstanceID].Next.CurrFrame;
    nextFrame[1] = TweenFrames[input.InstanceID].Next.NextFrame;
    time[1] = TweenFrames[input.InstanceID].Next.Time;

    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[0], clip[0], 0));
        c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[0], clip[0], 0));
        c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[0], clip[0], 0));
        c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[0], clip[0], 0));
        curr = matrix(c0, c1, c2, c3);
        
        n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], clip[0], 0));
        n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], clip[0], 0));
        n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], clip[0], 0));
        n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], clip[0], 0));
        next = matrix(n0, n1, n2, n3);
        
        currAnim = lerp(curr, next, time[0]);
        
        
        [flatten]
        if (clip[1] >= 0)
        {
            c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[1], clip[1], 0));
            c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[1], clip[1], 0));
            c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[1], clip[1], 0));
            c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[1], clip[1], 0));
            curr = matrix(c0, c1, c2, c3);
        
            n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], clip[1], 0));
            n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], clip[1], 0));
            n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], clip[1], 0));
            n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], clip[1], 0));
            next = matrix(n0, n1, n2, n3);
        
            nextAnim = lerp(curr, next, time[1]);
            
            currAnim = lerp(currAnim, nextAnim, TweenFrames[input.InstanceID].TweenTime);
        }
        
        transform += mul(weights[i], currAnim);
    }
    
    world = mul(transform, input.Transform);
}

MeshOutput VS_Animation(VertexModel input)
{
    MeshOutput output;
    SetAnimationWorld(World, input);
    
    VS_GENERATE

    return output;
}

MeshOutput VS_PreRender_Reflection_Animation(VertexModel input)
{
    MeshOutput output = VS_Animation(input);
    
    output.Position = WorldPosition(input.Position);
    output.Position = mul(output.Position, Reflection);
    output.Position = mul(output.Position, Projection);
    
    return output;
}

DepthOutput VS_Depth_Animation(VertexModel input)
{
    DepthOutput output;
    
    SetAnimationWorld(World, input);
    VS_DEPTH_GENERATE
    
    return output;
}