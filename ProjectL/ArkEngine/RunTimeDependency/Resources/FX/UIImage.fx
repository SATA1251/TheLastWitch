//***************************************************************************************
// color.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

Texture2D gDiffuseMap;
float4 gColor;

SamplerState samAnisotropic
{
    Filter = MIN_MAG_MIP_LINEAR;

    MaxAnisotropy = 4;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

cbuffer cbPerObject
{
    float4x4 gWorldViewProj;
};

struct VertexIn
{
	float3 PosL  : POSITION;
    float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

struct PSOut
{
    float4 Diffuse : SV_Target;
};

struct PSOut2
{
    float4 Color : SV_Target6;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
    vout.Pos = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
    vout.Tex = vin.Tex;
    
    return vout;
}

PSOut PS(VertexOut pin)
{
    PSOut output;
    
    float3 diffuse = gDiffuseMap.Sample(samAnisotropic, pin.Tex).xyz;
    output.Diffuse = float4(diffuse, 1.0f);
        
    return output;
}

PSOut2 PS2(VertexOut pin)
{
    PSOut2 output;
    
    output.Color = gColor;
        
    return output;
}

technique11 UITech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS2()));
    }
}
