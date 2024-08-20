//***************************************************************************************
// color.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

Texture2D gDiffuseMap;
Texture2D gPositionMap;

float gTransParency;
float gTime;
int gIndex;
float3 gDonutCenter;
float gDonutRange;

SamplerState samAnisotropic
{
    Filter = MIN_MAG_MIP_LINEAR;

    MaxAnisotropy = 4;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

float2 ToTexcoord(float4 position)
{
    float4 ndcPos = position / position.w;
    
    float2 texCoord = float2((ndcPos.x + 1.0f) * 0.5f, (1.0f - ndcPos.y) * 0.5f);
    
    return texCoord;
}

cbuffer cbPerObject
{
    float4x4 gWorld;
    float4x4 gWorldViewProj;
};

struct VertexIn
{
	float3 PosL : POSITION;
    float2 Tex : TEXCOORD;
};

struct VertexOut
{
    float3 PosL : POSITION;
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD;
};

struct DecalOut
{
    float4 Diffuse : SV_Target5;
};

struct PSOut
{
    float4 Diffuse : SV_Target1;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    vout.PosL = float3(vin.PosL);
        
    // Transform updated position to homogeneous clip space
    vout.PosH = mul(float4(vout.PosL, 1.0f), gWorldViewProj);
    
    vout.Tex = vin.Tex;

    return vout;
}

DecalOut PSDecal(VertexOut pin)
{
    DecalOut output;
    
    output.Diffuse = float4(0.0f, 1.0f, gIndex, 1.0f);
    
    return output;
}

PSOut PSBasic(VertexOut pin)
{
    PSOut output;
        
    float4 newPos = mul(float4(pin.PosL, 1.0f), gWorld);
    
    if (newPos.x < -100.0f || newPos.x > 110.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
    if (newPos.z < -120.0f || newPos.z > 100.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
    
    // 텍스처 좌표를 사용하여 색상 샘플링
    output.Diffuse = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
    
    float alpha = saturate(gTime);
    
    output.Diffuse.a = alpha * gTransParency;
    
    output.Diffuse.a = 1.0f;
    
    return output;
}

PSOut PSHorizontal(VertexOut pin)
{
    PSOut output;
    
    float4 newPos = mul(float4(pin.PosL, 1.0f), gWorld);
    
    if (newPos.x < -100.0f || newPos.x > 110.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
    if (newPos.z < -120.0f || newPos.z > 100.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
   
    // 텍스처 좌표를 사용하여 색상 샘플링
    output.Diffuse = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
    
    float alpha = (0.0f - pin.Tex.x) + gTime;
    
    alpha = saturate(alpha);
    
    alpha *= gTransParency;
    
    output.Diffuse.a = alpha;
    
    return output;
}

PSOut PSVertical(VertexOut pin)
{
    PSOut output;
    
    float4 newPos = mul(float4(pin.PosL, 1.0f), gWorld);
    
    if (newPos.x < -110.0f || newPos.x > 110.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
    if (newPos.z < -130.0f || newPos.z > 120.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
   
    // 텍스처 좌표를 사용하여 색상 샘플링
    output.Diffuse = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
    
    float alpha = (pin.Tex.y - 1.0f) + gTime;
    
    alpha = saturate(alpha);
    
    alpha *= gTransParency;
    
    output.Diffuse.a = alpha;
    
    if (alpha >= 0.5)
    {
        output.Diffuse.r = output.Diffuse.r + 0.5f;
    }

    
    return output;
}

PSOut PSCenter(VertexOut pin)
{
    PSOut output;
    
    float4 newPos = mul(float4(pin.PosL, 1.0f), gWorld);
    
    if (newPos.x < -100.0f || newPos.x > 110.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
    if (newPos.z < -120.0f || newPos.z > 100.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
   
    // 텍스처 좌표를 사용하여 색상 샘플링
    output.Diffuse = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
    
    float distanceFromCenter = length(pin.Tex - float2(0.5f, 0.5f));
    
    float maxDistance = length(float2(0.0f, 0.0f) - float2(0.5f, 0.5f));
    
    float alpha = (-1 * (distanceFromCenter + 0.5f)) + gTime;
    
    alpha = saturate(alpha);
    
    alpha *= gTransParency;
    
    output.Diffuse.a = alpha;
    
    return output;
}

PSOut PSCenterWithLine(VertexOut pin)
{
    PSOut output;
    
    float4 newPos = mul(float4(pin.PosL, 1.0f), gWorld);
    
    if (newPos.x < -100.0f || newPos.x > 110.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
    if (newPos.z < -120.0f || newPos.z > 100.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
   
    // 텍스처 좌표를 사용하여 색상 샘플링
    output.Diffuse = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
    
    float distanceFromCenter = length(pin.Tex - float2(0.5f, 0.5f));
    
    if (distanceFromCenter >= 0.499f)
    {
        output.Diffuse.a = 1.0f;
    }
    
    else
    {
        float maxDistance = length(float2(0.0f, 0.0f) - float2(0.5f, 0.5f));
    
        float alpha = (-1 * (distanceFromCenter + 0.5f)) + gTime;
    
        alpha = saturate(alpha);
    
        alpha *= gTransParency;
            
        output.Diffuse.a = alpha;
    }
    
    return output;
}

PSOut PsDonut(VertexOut pin)
{
    PSOut output;
   
    float4 newPos = mul(float4(pin.PosL, 1.0f), gWorld);
    
    if (newPos.x < -100.0f || newPos.x > 110.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
    if (newPos.y < -120.0f || newPos.y > 100.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
    
    float distance = length(float2(newPos.x, newPos.z) - float2(gDonutCenter.x, gDonutCenter.z));
            
    output.Diffuse = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
    
    if (distance < gDonutRange)
    {
        output.Diffuse.a = 0.0f;
    }
    else
    {
        float alpha = saturate(gTime);
    
        output.Diffuse.a = alpha * gTransParency;
    }
    
    return output;
}

PSOut PsDonutInsideEmpty(VertexOut pin)
{
    PSOut output;
       
    float4 newPos = mul(float4(pin.PosL, 1.0f), gWorld);
    
    if (newPos.x < -100.0f || newPos.x > 110.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
    if (newPos.z < -120.0f || newPos.z > 100.0f)
    {
        output.Diffuse.a = 0.0f;
        return output;
    }
    
    float distance = length(float2(newPos.x, newPos.z) - float2(gDonutCenter.x, gDonutCenter.z));
            
    output.Diffuse = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
    
    if (distance < gDonutRange)
    {
        output.Diffuse.a = 0.0f;
    }
    else
    {
        float alpha = saturate(gTime);
    
        output.Diffuse.a = alpha * gTransParency;
    }
    return output;
}

technique11 Tech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSBasic()));
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSHorizontal()));
    }

    pass P2
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader(CompileShader(ps_5_0, PSVertical()));
    }

    pass P3
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSCenter()));
    }

    pass P4
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSCenterWithLine()));
    }

    pass P5
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PsDonut()));
    }

    pass P6
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PsDonutInsideEmpty()));
    }

    pass P7
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSDecal()));
    }
}
