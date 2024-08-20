//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

// ���� �޴� ��ä�� ������
Texture2D FinalTexture;

SamplerState samAnisotropic
{
    Filter = MIN_MAG_MIP_LINEAR;
    MaxAnisotropy = 16;

    AddressU = ClAMP;
    AddressV = ClAMP;

    MipLODBias = 0.0;
    MinLOD = 0;
    MaxLOD = 15;
};

struct VertexIn
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

struct VertexOut
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

void GetGBufferAttributes(float2 texCoord, out float4 finalTexture, float scale)
{
    float2 scaledTexCoord = texCoord * float2(scale, scale);
    finalTexture = FinalTexture.Sample(samAnisotropic, scaledTexCoord);
}

float gaussWeight(int x)
{
    float sigma = 5.0f;
    return exp(-float(x * x) / (2.0f * sigma * sigma)) / (sqrt(2.0f * 3.14159265f) * sigma);
}

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    vout.Pos = float4(vin.Pos, 1.0f);
    vout.Tex = vin.Tex;
    
    return vout;
}

float4 PSGrayScale(VertexOut pin) : SV_Target
{
    float4 finalTexture;
    
    // GetGBufferAttributes �Լ��� ����Ͽ� �ؽ�ó ���ø�
    GetGBufferAttributes(pin.Tex, finalTexture, 4.0f);

    // Rec. 709 ����ġ�� ����Ͽ� �׷��̽����� ��ȯ
    float3 grayWeights = float3(0.2126f, 0.7152f, 0.0722f);
    float grayValue = dot(finalTexture.rgb, grayWeights);
    
    float4 result = lerp(float4(0.0f, 0.0f, 0.0f, 1.0f), finalTexture, step(0.5f, grayValue));
    
    //result *= 2.0f;
    
    return result;
    
    //// �׷��̽����� �� ����
    //float4 grayColor = float4(grayValue, grayValue, grayValue, finalTexture.a);
    //
    //// �� ������ ���� �� ���� �� Ŭ����
    //grayColor.rgb = saturate(grayColor.rgb - 0.5f) * 2.0f;
    //
    //return grayColor;
}

float4 PSBlurVertical(VertexOut pin) : SV_Target
{
    float4 sum = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float weightSum = 0.0f;
    
    float weights[9] = { 0.028532f, 0.067234f, 0.124009f, 0.179044f, 0.20236f, 0.179044f, 0.124009f, 0.067234f, 0.028532f };
    
    // 1 / 1440.0f;
    float2 texelSize = float2(0.0f, 0.00069444444);
    
    // ����þ� ���� ����
    for (int i = -4; i <= 4; ++i)
    {
        float weight = gaussWeight(i);
        
        // �ؽ�ó ��ǥ�� 4�� �����ϸ��� �����Ͽ� ���ø�
        sum += FinalTexture.Sample(samAnisotropic, (pin.Tex + float2(0.0f, i) * texelSize) * float2(4.0f, 4.0f)) * weight;
        weightSum += weight;
    }
    
    // ������ ��� ��ȯ
    return sum / weightSum;
}

float4 PSBlurHorizontal(VertexOut pin) : SV_Target
{
    float4 sum = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float weightSum = 0.0f;
    
    float weights[9] = { 0.028532f, 0.067234f, 0.124009f, 0.179044f, 0.20236f, 0.179044f, 0.124009f, 0.067234f, 0.028532f };
    
    // 1 / 2560.0f;
    float2 texelSize = float2(0.000390625, 0.0f);
    
    // ����þ� ���� ����
    for (int i = -4; i <= 4; ++i)
    {
        float weight = gaussWeight(i);
        
        // �ؽ�ó ��ǥ�� 4�� �����ϸ��� �����Ͽ� ���ø�
        sum += FinalTexture.Sample(samAnisotropic, (pin.Tex + float2(i, 0.0f) * texelSize) * float2(4.0f, 4.0f)) * weight;
        weightSum += weight;
    }
    
    // ������ ��� ��ȯ
    return sum / weightSum;
}

technique11 Final
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSGrayScale()));
    }
    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSBlurHorizontal()));
    }
    pass P2
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSBlurVertical()));
    }
}