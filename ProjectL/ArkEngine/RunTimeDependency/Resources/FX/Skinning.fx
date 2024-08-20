//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

#include "LightHelper.fx"

cbuffer cbPerFrame
{
    DirectionalLight gDirLights[3];
    int gLightCount;
    bool gHaveTangent;
    
    float3 gEyePosW;

    float gFogStart;
    float gFogRange;
    float4 gFogColor;
};

cbuffer cbPerObject
{
    float4x4 gWorld;
    float4x4 gWorldInvTranspose;
    float4x4 gWorldViewProj;
    float4x4 gTexTransform;
    Material gMaterial;
};

cbuffer cbSkinned
{
    float4x4 gBoneTransforms[96];
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
TextureCube gCubeMap;
Texture2D gNormalMap;

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;

    AddressU = WRAP;
    AddressV = WRAP;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 Tex : TEXCOORD;
    float3 TangentL : TANGENT;

    float3 Weight : BLENDWEIGHT;

    uint4 BoneIndicles : BLENDINDICES;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 Tex : TEXCOORD;
    float3 TangentW : TANGENT;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = vin.Weight.x;
    weights[1] = vin.Weight.y;
    weights[2] = vin.Weight.z;
    weights[3] = 1 - weights[0] - weights[1] - weights[2];

    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);

    posL += weights[0] * mul(float4(vin.PosL, 1.0F), gBoneTransforms[vin.BoneIndicles.x]).xyz;
    normalL += weights[0] * mul(vin.NormalL, (float3x3) gBoneTransforms[vin.BoneIndicles.x]);

    posL += weights[1] * mul(float4(vin.PosL, 1.0F), gBoneTransforms[vin.BoneIndicles.y]).xyz;
    normalL += weights[1] * mul(vin.NormalL, (float3x3) gBoneTransforms[vin.BoneIndicles.y]);

    posL += weights[2] * mul(float4(vin.PosL, 1.0F), gBoneTransforms[vin.BoneIndicles.z]).xyz;
    normalL += weights[2] * mul(vin.NormalL, (float3x3) gBoneTransforms[vin.BoneIndicles.z]);

    posL += weights[3] * mul(float4(vin.PosL, 1.0F), gBoneTransforms[vin.BoneIndicles.w]).xyz;
    normalL += weights[3] * mul(vin.NormalL, (float3x3) gBoneTransforms[vin.BoneIndicles.w]);


	// Transform to world space space.
    vout.PosW = mul(float4(posL, 1.0f), gWorld).xyz;
    vout.NormalW = mul(normalL, (float3x3) gWorldInvTranspose);
    vout.TangentW = mul(vin.TangentL, (float3x3) gWorld);
    
	// Transform to homogeneous clip space.
    vout.PosH = mul(float4(posL, 1.0f), gWorldViewProj);

	// Output vertex attributes for interpolation across triangle.
    vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
	///vout.Tex = vin.Tex;

    return vout;
}

float4 PS(VertexOut pin, uniform bool gUseTexure, uniform bool gReflect) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW);

// The toEye vector is used in lighting.
    float3 toEye = gEyePosW - pin.PosW;

// Cache the distance to the eye from this surface point.
    float distToEye = length(toEye);

// Normalize.
    toEye /= distToEye;

// Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);

    if (gUseTexure)
    {
	// Sample texture.
        texColor = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
    }

//
// Lighting.
//
    float3 bumpedNormalW = { 0.0f, 0.0f, 0.0f };
    
    if (gHaveTangent)
    {
        float3 normalMapSample = gNormalMap.Sample(samAnisotropic, pin.Tex).rgb;
        bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, pin.NormalW, pin.TangentW);
    }
	
    float4 litColor = texColor;
    if (gLightCount > 0)
    {
	// Start with a sum of zero. 
        float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.  
	[unroll]
        for (int i = 0; i < gLightCount; ++i)
        {
            float4 A, D, S;
            if (gHaveTangent == true)
            {
                ComputeDirectionalLight(gMaterial, gDirLights[i], bumpedNormalW, toEye,
				A, D, S);
            }
            else
            {
                ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEye,
				A, D, S);
            }

            ambient += A;
            diffuse += D;
            spec += S;
        }

	// Modulate with late add.
        litColor = texColor * (ambient + diffuse) + spec;
        
        if (gReflect == true)
        {
            float3 incident = -toEye;
            
            float3 reflectionVector = { 0.0f, 0.0f, 0.0f };
            if (gHaveTangent == true)
            {
                reflectionVector = reflect(incident, bumpedNormalW);
            }
            else
            {
                reflectionVector = reflect(incident, pin.NormalW);
            }
            
            float4 reflectionColor = gCubeMap.Sample(samAnisotropic, reflectionVector);

            litColor += gMaterial.Reflect * reflectionColor;
        }
    }

// Common to take alpha from diffuse material and texture.
    litColor.a = gMaterial.Diffuse.a * texColor.a;

//litColor.r = 1.0f;
//litColor.g = 0.0f;
//litColor.b = 0.0f;

    return litColor;
}

technique11 Light
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(false, false)));
    }
}

technique11 LightTex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(true, false)));
    }
}

technique11 LightTexReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(true, true)));
    }
}