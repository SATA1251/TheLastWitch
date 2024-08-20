//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

#include "LightHelper.fx"

cbuffer cbPerFrame
{
    float3 gEyePosW;
    
    // 방출 위치나 방향이 다른 경우를 위한 변수들
    float3 gEmitPosW;
    float3 gEmitDirW;
    float gGameTime;
    float gTimeStep;
    float4x4 gViewProj;
    
    float3 gEmitVelocity;
    float2 gParticleSize;
    bool gIsRandom;
    float gParticleFadeTime; // 파티클이 투명해지는 시간
    float gParticleLifeTime; // 파티클이 유지되는 시간
    float3 gParticleColor; // 파티클의 색상
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
    // 입자 가속을 위한 알짜 상수 가속도
    //float3 gAccelW = { 0.0f, 7.8f, 0.0f };
    
    // 텍스쳐를 사각형 전체에 입히는 텍스쳐 좌표들
    // 점 입자를 사각형으로 확장할 때 쓰임
    float2 gQuadTex[4] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f)
    };
};

// 입자 텍스처 적용을 위한 텍스쳐 배열
Texture2DArray gTexArray : register(t0);
//SamplerState gSampler : register(s0);

// 쉐이더에서 난수를 얻는 데 쓰이는 무작위 텍스쳐
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

// 회전 행렬 계산 함수
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

// 보조 함수
float3 RandUnitVec3(float offset)
{
    // 게임 시간 더하기 오프셋을 무작위 텍스처 추출을 위한
    // 텍스처 좌표로 사용한다
    float u = (gGameTime + offset);
    
    // 벡터 성분들의 범위는 [-1, 1]이다
    float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
    
    // 단위 구로 투영한다
    return normalize(v);
    //return v;
}

float3 RandNum(float offset)
{
    // 게임 시간 더하기 오프셋을 무작위 텍스처 추출을 위한
    // 텍스처 좌표로 사용한다
    float u = (gGameTime + offset);
    
    // 벡터 성분들의 범위는 [-1, 1]이다
    float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
    
    float3 scaledVector = 0.25 * (v + float3(1.0, 1.0, 1.0)) + 0.5;
    
    // 단위 구로 투영한다
    return normalize(scaledVector);
    //return v;
}

// 스트림 출력 전용 기법
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

// 스트림 출력 전용 기하 쉐이더는 개 입자의 방출과 기존 입자의
// 파괴만 담당한다. 입자 시스템마다 입자의 생성, 파괴 규칙이 다를 것이므로,
// 이 부분의 구현 역시 입자 시스템마다 다를 필요가 있다.
[maxvertexcount(2)]
void StreamOutGS(point Particle gin[1], inout PointStream<Particle> ptStream)
{
    gin[0].Age += gTimeStep;
    
    if (gin[0].Type == PT_EMITTER)
    {
        // 새 입자를 방출할 시간이 되었는가?
        if (gin[0].Age > 0.005f)
        {
            float3 vRandom = RandUnitVec3(0.0f);
            vRandom.x *= 0.5f;
            vRandom.y *= 0.5f;
            vRandom.z *= 0.5f;
            
            Particle p;
            p.InitialPosW = gEmitPosW.xyz;

            // 회전 행렬을 계산하고 파티클을 회전시키는 예시
            
            if (gIsRandom == true)
            {
                p.InitialVelW = gEmitVelocity * vRandom * gEmitDirW;
            }
            else
            {
                p.InitialVelW = gEmitVelocity * gEmitDirW;
            }
           

            p.SizeW = gParticleSize; // 파티클 사각형의 크기를 정한다
            p.Age = 0.0f;
            p.Type = PT_FLARE;
           
            ptStream.Append(p);
           
           // 방출할 시간을 재설정한다
            gin[0].Age = 0.0f;
            
        }
        
        // 방출기 입자 하나는 항상 유지한다
        ptStream.Append(gin[0]);
    }
    else
    {
        // 여기에서 입자를 유지할 조건들을 지정한다
        // 구체적인 조건은 입자 시스템마다 다를 수 있다
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

        // 스트림 출력 전용을 위해 픽셀 쉐이더를 비활성화 한다
        SetPixelShader(NULL);

        // 스트림 출력 전용을 위해서는 깊이 버퍼도 비활성화 해야한다
        SetDepthStencilState(DisableDepth, 0);
    }
}

/// 렌더링 기법
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

        // 로컬 회전을 위한 회전 행렬 계산
    
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
    
    // 초기 속도 계산
    float3 initialVelRotated = mul(mul(mul(vin.InitialVelW, rotationMatrixX), rotationMatrixY), rotationMatrixZ);



    
    // 상수 가속 공식
    vout.PosW = 0.5f * t * t * gAccelW + t * initialVelRotated + initialPosRotated;
    
    // 시간에 따른 색상 감소
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

// 렌더링용 기하 쉐이더는 그냥 점을 카메라를 향한 사각형으로 확장한다
[maxvertexcount(4)]
void DrawGS(point VertexOut gin[1], inout TriangleStream<GeoOut> triStream)
{
   // 방출기 입자는 그리지 않는다
    if (gin[0].Type != PT_EMITTER)
    {
        // 빌보드가 카메라를 향하게 하는 세계 행렬을 계산한다
        float3 look = normalize(gEyePosW.xyz - gin[0].PosW);
        float3 right = normalize(cross(float3(0, 1, 0), look));
        float3 up = cross(look, right);

        // 각도를 도 단위로 지정하고 라디안으로 변환
        float angleX = gParticleRotateAngle.x * 3.14159265f / 180.0f;
        float angleY = gParticleRotateAngle.y * 3.14159265f / 180.0f;
        float angleZ = gParticleRotateAngle.z * 3.14159265f / 180.0f;

        // x 축 주위 회전 행렬
        float3x3 rotateX = float3x3(
            1.0f, 0.0f, 0.0f,
            0.0f, cos(angleX), -sin(angleX),
            0.0f, sin(angleX), cos(angleX)
        );

        // y 축 주위 회전 행렬
        float3x3 rotateY = float3x3(
            cos(angleY), 0.0f, sin(angleY),
            0.0f, 1.0f, 0.0f,
            -sin(angleY), 0.0f, cos(angleY)
        );

        // z 축 주위 회전 행렬
        float3x3 rotateZ = float3x3(
            cos(angleZ), -sin(angleZ), 0.0f,
            sin(angleZ), cos(angleZ), 0.0f,
            0.0f, 0.0f, 1.0f
        );

        // 회전 행렬을 이용하여 right, up, look 벡터를 회전시킴
        right = mul(rotateX, right);
        up = mul(rotateX, up);

        right = mul(rotateY, right);
        up = mul(rotateY, up);

        right = mul(rotateZ, right);
        up = mul(rotateZ, up);

        // 사각형을 구성하는 삼각형 띠 정점들을 계산
        float halfWidth = 0.5f * gin[0].SizeW.x;
        float halfHeight = 0.5f * gin[0].SizeW.y;

        float4 v[4];
        v[0] = float4(gin[0].PosW + halfWidth * right - halfHeight * up, 1.0f);
        v[1] = float4(gin[0].PosW + halfWidth * right + halfHeight * up, 1.0f);
        v[2] = float4(gin[0].PosW - halfWidth * right - halfHeight * up, 1.0f);
        v[3] = float4(gin[0].PosW - halfWidth * right + halfHeight * up, 1.0f);

        // 이 정점들을 세계 공간으로 변환하고, 하나의 삼각형 띠로서 출력
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