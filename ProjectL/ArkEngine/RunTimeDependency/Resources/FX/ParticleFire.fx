//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

#include "LightHelper.fx"

cbuffer cbPerFrame
{
    float3 gEyePosW;
    
    // ���� ��ġ�� ������ �ٸ� ��츦 ���� ������
    float3 gEmitPosW;
    float3 gEmitDirW;
    float gGameTime;
    float gTimeStep;
    float4x4 gViewProj;
    
    float3 gEmitVelocity;
    float2 gParticleSize;
    bool gIsRandom;
    float gParticleFadeTime; // ��ƼŬ�� ���������� �ð�
    float gParticleLifeTime; // ��ƼŬ�� �����Ǵ� �ð�
    float3 gParticleColor; // ��ƼŬ�� ����
    float3 gParticleRoatation;
    float3 gAccelW;
    
    float3 gRotationAxis;
    float3 gRotationAngle;
    float gRotationAngleY;
    float gRotationAngleZ;
    
    float3 gParticleRotateAngle;
};

cbuffer cbFixed
{
    // ���� ������ ���� ��¥ ��� ���ӵ�
    //float3 gAccelW = { 0.0f, 7.8f, 0.0f };
    
    // �ؽ��ĸ� �簢�� ��ü�� ������ �ؽ��� ��ǥ��
    // �� ���ڸ� �簢������ Ȯ���� �� ����
    float2 gQuadTex[4] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f)
    };
};

// ���� �ؽ�ó ������ ���� �ؽ��� �迭
Texture2DArray gTexArray : register(t0);
//SamplerState gSampler : register(s0);

// ���̴����� ������ ��� �� ���̴� ������ �ؽ���
Texture1D gRandomTex;

SamplerState samLinear : register(s0)
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = WRAP;
    AddressV = WRAP;
};

DepthStencilState DisableDepth
{
    DepthEnable = false;
    DepthWriteMask = ZERO;
};

DepthStencilState NoDepthWrites
{
    DepthEnable = true;
    DepthWriteMask = ZERO;
};

BlendState AdditiveBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[1] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ONE;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[1] = 0x0F;
};

BlendState AdditiveBlendingForFoward
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ONE;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

// ȸ�� ��� ��� �Լ�
float3x3 RotateAxisAngle(float3 axis, float angle)
{
    float3 normalizedAxis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float3 temp = (1.0f - c) * normalizedAxis;

    float3x3 rotationMatrix = float3x3(
        c + temp.x * normalizedAxis.x, temp.x * normalizedAxis.y + s * normalizedAxis.z, temp.x * normalizedAxis.z - s * normalizedAxis.y,
        temp.y * normalizedAxis.x - s * normalizedAxis.z, c + temp.y * normalizedAxis.y, temp.y * normalizedAxis.z + s * normalizedAxis.x,
        temp.z * normalizedAxis.x + s * normalizedAxis.y, temp.z * normalizedAxis.y - s * normalizedAxis.x, c + temp.z * normalizedAxis.z
    );

    return rotationMatrix;
}

// ���� �Լ�
float3 RandUnitVec3(float offset)
{
    // ���� �ð� ���ϱ� �������� ������ �ؽ�ó ������ ����
    // �ؽ�ó ��ǥ�� ����Ѵ�
    float u = (gGameTime + offset);
    
    // ���� ���е��� ������ [-1, 1]�̴�
    float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
    
    // ���� ���� �����Ѵ�
    return normalize(v);
    //return v;
}

float3 RandNum(float offset)
{
    // ���� �ð� ���ϱ� �������� ������ �ؽ�ó ������ ����
    // �ؽ�ó ��ǥ�� ����Ѵ�
    float u = (gGameTime + offset);
    
    // ���� ���е��� ������ [-1, 1]�̴�
    float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
    
    float3 scaledVector = 0.25 * (v + float3(1.0, 1.0, 1.0)) + 0.5;
    
    // ���� ���� �����Ѵ�
    return normalize(scaledVector);
    //return v;
}

// ��Ʈ�� ��� ���� ���
#define PT_EMITTER 0
#define PT_FLARE 1
#define PT_LASER 2

struct Particle
{
    float3 InitialPosW : POSITION;
    float3 InitialVelW : VELOCITY;
    
    float2 SizeW : SIZE;
    float Age : AGE;
    uint Type : TYPE;
};

Particle StreamOutVS(Particle vin)
{
    return vin;
}


float GetRadian(float angle)
{
    float radian = 2.0f * 3.141592f * (angle / 360);
    
    return radian;
}

// ��Ʈ�� ��� ���� ���� ���̴��� �� ������ ����� ���� ������
// �ı��� ����Ѵ�. ���� �ý��۸��� ������ ����, �ı� ��Ģ�� �ٸ� ���̹Ƿ�,
// �� �κ��� ���� ���� ���� �ý��۸��� �ٸ� �ʿ䰡 �ִ�.
[maxvertexcount(2)]
void StreamOutGS(point Particle gin[1], inout PointStream<Particle> ptStream)
{
    gin[0].Age += gTimeStep;
    
    if (gin[0].Type == PT_EMITTER)
    {
        // �� ���ڸ� ������ �ð��� �Ǿ��°�?
        if (gin[0].Age > 0.005f)
        {
            float3 vRandom = RandUnitVec3(0.0f);
            vRandom.x *= 0.5f;
            vRandom.y *= 0.5f;
            vRandom.z *= 0.5f;
            
            Particle p;
            p.InitialPosW = gEmitPosW.xyz;

            // ȸ�� ����� ����ϰ� ��ƼŬ�� ȸ����Ű�� ����
            
            if (gIsRandom == true)
            {
                p.InitialVelW = gEmitVelocity * vRandom * gEmitDirW;
            }
            else
            {
                p.InitialVelW = gEmitVelocity * gEmitDirW;
            }
           

            p.SizeW = gParticleSize; // ��ƼŬ �簢���� ũ�⸦ ���Ѵ�
            p.Age = 0.0f;
            p.Type = PT_FLARE;
           
            ptStream.Append(p);
           
           // ������ �ð��� �缳���Ѵ�
            gin[0].Age = 0.0f;
            
        }
        
        // ����� ���� �ϳ��� �׻� �����Ѵ�
        ptStream.Append(gin[0]);
    }
    else
    {
        // ���⿡�� ���ڸ� ������ ���ǵ��� �����Ѵ�
        // ��ü���� ������ ���� �ý��۸��� �ٸ� �� �ִ�
        if (gin[0].Age <= gParticleLifeTime)
            ptStream.Append(gin[0]);
    }
}

GeometryShader gsStreamOut = ConstructGSWithSO(CompileShader(gs_5_0, StreamOutGS()), "POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x");

technique11 StreamOutTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, StreamOutVS()));
        SetGeometryShader(gsStreamOut);

        // ��Ʈ�� ��� ������ ���� �ȼ� ���̴��� ��Ȱ��ȭ �Ѵ�
        SetPixelShader(NULL);

        // ��Ʈ�� ��� ������ ���ؼ��� ���� ���۵� ��Ȱ��ȭ �ؾ��Ѵ�
        SetDepthStencilState(DisableDepth, 0);
    }
}

/// ������ ���
struct VertexOut
{
    float3 PosW : POSITION;
    float2 SizeW : SIZE;
    float4 Color : COLOR;
    uint Type : TYPE;
};


VertexOut DrawVS(Particle vin)
{
    VertexOut vout;
    
    float t = vin.Age;

        // ���� ȸ���� ���� ȸ�� ��� ���
    
    float radiX = GetRadian(gRotationAngle.x);
    //float radiX = GetRadian(90.0f);
    float radiY = GetRadian(gRotationAngle.y);
    //float radiY = GetRadian(90.0f);
    float radiZ = GetRadian(gRotationAngle.z);
    //float radiZ = GetRadian(45.0f);
    
    
    float3x3 rotationMatrixX = RotateAxisAngle(float3(1.0f, 0.0f, 0.0f), radiX);
    float3x3 rotationMatrixY = RotateAxisAngle(float3(0.0f, 1.0f, 0.0f), radiY);
    float3x3 rotationMatrixZ = RotateAxisAngle(float3(0.0f, 0.0f, 1.0f), radiZ);

    float3 initialPos = vin.InitialPosW - gEmitPosW;
    float3 initialPosRotated = mul(mul(mul(initialPos, rotationMatrixX), rotationMatrixY), rotationMatrixZ) + gEmitPosW;
    
    // �ʱ� �ӵ� ���
    float3 initialVelRotated = mul(mul(mul(vin.InitialVelW, rotationMatrixX), rotationMatrixY), rotationMatrixZ);



    
    // ��� ���� ����
    vout.PosW = 0.5f * t * t * gAccelW + t * initialVelRotated + initialPosRotated;
    
    // �ð��� ���� ���� ����
    float opacity = 1.0f - smoothstep(0.0f, 1.0f, t / gParticleFadeTime);
    vout.Color = float4(gParticleColor, opacity);
    
    vout.SizeW = vin.SizeW;
    vout.Type = vin.Type;

    return vout;

}


struct GeoOut
{
    float4 PosH : SV_Position;

    float4 Color : COLOR;

    float2 Tex : TEXCOORD0;
};

// �������� ���� ���̴��� �׳� ���� ī�޶� ���� �簢������ Ȯ���Ѵ�
[maxvertexcount(4)]
void DrawGS(point VertexOut gin[1], inout TriangleStream<GeoOut> triStream)
{
   // ����� ���ڴ� �׸��� �ʴ´�
    if (gin[0].Type != PT_EMITTER)
    {
        // �����尡 ī�޶� ���ϰ� �ϴ� ���� ����� ����Ѵ�
        float3 look = normalize(gEyePosW.xyz - gin[0].PosW);
        float3 right = normalize(cross(float3(0, 1, 0), look));
        float3 up = cross(look, right);

        // ������ �� ������ �����ϰ� �������� ��ȯ
        float angleX = gParticleRotateAngle.x * 3.14159265f / 180.0f;
        float angleY = gParticleRotateAngle.y * 3.14159265f / 180.0f;
        float angleZ = gParticleRotateAngle.z * 3.14159265f / 180.0f;

        // x �� ���� ȸ�� ���
        float3x3 rotateX = float3x3(
            1.0f, 0.0f, 0.0f,
            0.0f, cos(angleX), -sin(angleX),
            0.0f, sin(angleX), cos(angleX)
        );

        // y �� ���� ȸ�� ���
        float3x3 rotateY = float3x3(
            cos(angleY), 0.0f, sin(angleY),
            0.0f, 1.0f, 0.0f,
            -sin(angleY), 0.0f, cos(angleY)
        );

        // z �� ���� ȸ�� ���
        float3x3 rotateZ = float3x3(
            cos(angleZ), -sin(angleZ), 0.0f,
            sin(angleZ), cos(angleZ), 0.0f,
            0.0f, 0.0f, 1.0f
        );

        // ȸ�� ����� �̿��Ͽ� right, up, look ���͸� ȸ����Ŵ
        right = mul(rotateX, right);
        up = mul(rotateX, up);

        right = mul(rotateY, right);
        up = mul(rotateY, up);

        right = mul(rotateZ, right);
        up = mul(rotateZ, up);

        // �簢���� �����ϴ� �ﰢ�� �� �������� ���
        float halfWidth = 0.5f * gin[0].SizeW.x;
        float halfHeight = 0.5f * gin[0].SizeW.y;

        float4 v[4];
        v[0] = float4(gin[0].PosW + halfWidth * right - halfHeight * up, 1.0f);
        v[1] = float4(gin[0].PosW + halfWidth * right + halfHeight * up, 1.0f);
        v[2] = float4(gin[0].PosW - halfWidth * right - halfHeight * up, 1.0f);
        v[3] = float4(gin[0].PosW - halfWidth * right + halfHeight * up, 1.0f);

        // �� �������� ���� �������� ��ȯ�ϰ�, �ϳ��� �ﰢ�� ��μ� ���
        GeoOut gout;
        [unroll]
        for (int i = 0; i < 4; ++i)
        {
            gout.PosH = mul(v[i], gViewProj);
            gout.Tex = gQuadTex[i];
            gout.Color = gin[0].Color;

            triStream.Append(gout);
        }
    }
}

float4 DrawPS(GeoOut pin) : SV_Target1
{
    return gTexArray.Sample(samLinear, float3(pin.Tex, 0)) * pin.Color;
}

float4 DrawPSForForward(GeoOut pin) : SV_Target0
{
    return gTexArray.Sample(samLinear, float3(pin.Tex, 0)) * pin.Color;
}

technique11 DrawTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, DrawVS()));
        SetGeometryShader(CompileShader(gs_5_0, DrawGS()));
        SetPixelShader(CompileShader(ps_5_0, DrawPS()));

        SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetDepthStencilState(NoDepthWrites, 0);
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, DrawVS()));
        SetGeometryShader(CompileShader(gs_5_0, DrawGS()));
        SetPixelShader(CompileShader(ps_5_0, DrawPSForForward()));

        SetBlendState(AdditiveBlendingForFoward, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetDepthStencilState(NoDepthWrites, 0);
    }
}