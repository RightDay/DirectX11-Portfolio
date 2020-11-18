struct InputDesc
{
    matrix Bone;
};
StructuredBuffer<InputDesc> Input;

struct OutputDesc
{
    matrix Result;
};
RWStructuredBuffer<OutputDesc> Output;


#define MAX_MODEL_TRANSFORMS 250
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

cbuffer CB_Attach
{
    uint AttachBoneIndex;
};

[numthreads(MAX_MODEL_INSTANCE, 1, 1)]
void CS(uint GroupIndex : SV_GroupIndex)
{
    uint index = GroupIndex;

    uint boneIndex[2];
    matrix result = 0;
    
    boneIndex[0] = (TweenFrames[index].Curr.Clip * MAX_MODEL_KEYFRAMES * MAX_MODEL_TRANSFORMS);
    boneIndex[0] += (TweenFrames[index].Curr.CurrFrame * MAX_MODEL_TRANSFORMS);
    boneIndex[0] += AttachBoneIndex;
    
    boneIndex[1] = (TweenFrames[index].Curr.Clip * MAX_MODEL_KEYFRAMES * MAX_MODEL_TRANSFORMS);
    boneIndex[1] += (TweenFrames[index].Curr.NextFrame * MAX_MODEL_TRANSFORMS);
    boneIndex[1] += AttachBoneIndex;
    
    matrix currFrame = Input[boneIndex[0]].Bone;
    matrix nextFrame = Input[boneIndex[1]].Bone;
    
    result = lerp(currFrame, nextFrame, TweenFrames[index].Curr.Time);
    
    
    [flatten]
    if (TweenFrames[index].Next.Clip > -1)
    {
        boneIndex[0] = (TweenFrames[index].Next.Clip * MAX_MODEL_KEYFRAMES * MAX_MODEL_TRANSFORMS);
        boneIndex[0] += (TweenFrames[index].Next.CurrFrame * MAX_MODEL_TRANSFORMS);
        boneIndex[0] += AttachBoneIndex;

        boneIndex[1] = (TweenFrames[index].Next.Clip * MAX_MODEL_KEYFRAMES * MAX_MODEL_TRANSFORMS);
        boneIndex[1] += (TweenFrames[index].Next.NextFrame * MAX_MODEL_TRANSFORMS);
        boneIndex[1] += AttachBoneIndex;

        currFrame = Input[boneIndex[0]].Bone;
        nextFrame = Input[boneIndex[1]].Bone;

        matrix nextAnim = lerp(currFrame, nextFrame, TweenFrames[index].Next.Time);
        
        result = lerp(result, nextAnim, TweenFrames[index].TweenTime);
    }
    
    Output[index].Result = result;
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}