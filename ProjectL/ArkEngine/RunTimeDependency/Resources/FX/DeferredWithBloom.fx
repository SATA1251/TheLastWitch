//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

// ºûÀ» ¹Þ´Â °´Ã¤ÀÇ Æ÷Áö¼Ç
Texture2D FinalTexture;

Texture2D BlurTexture;

Texture2D BlurGrayTexture;

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

float3 ToneMapReinhard(float3 color)
{
    float3 mappedColor = color / (color + float3(1.0, 1.0, 1.0));
    return mappedColor;
}

void GetGBufferAttributes(float2 texCoord, out float4 finalTexture, out float4 blur, out float4 blurGray)
{
    finalTexture = FinalTexture.Sample(samAnisotropic, texCoord);
    
    blur = BlurTexture.Sample(samAnisotropic, texCoord);
    
    blurGray = BlurGrayTexture.Sample(samAnisotropic, texCoord);
}

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    vout.Pos = float4(vin.Pos, 1.0f);
    vout.Tex = vin.Tex;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float4 finalTexture;
    
    float4 blur;
    
    float4 blurGrayScale;
    
    GetGBufferAttributes(pin.Tex, finalTexture, blur, blurGrayScale);
    
    float4 result = finalTexture + blurGrayScale;
    
    return finalTexture + blurGrayScale;
    
    //float3 toneMappedColor = ToneMapReinhard(result.xyz);
    //
    //return float4(toneMappedColor, result.a);
    
    //// Èå¸²+¹àÀº»ö °­Á¶ : Èå¸² Ã³¸® + ¹àÀº»ö ÃßÃâ 
    //float4 result = blur + blurGrayScale;
    //
    //// ¿øº» È¥ÇÕ : Èå¸²,ºû°­Á¶ * ¿øº»
    //result *= finalTexture;
    //
    //// ¿øº» °­Á¶ : ¿øº» È¥ÇÕ * ¿øº»
    //result += finalTexture;
    //
    //float3 toneMappedColor = ToneMapReinhard(result.xyz);
    //
    //return float4(toneMappedColor, result.a);
    //
    //return result;
}

technique11 Final
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}