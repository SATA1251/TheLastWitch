//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

#include "LightHelper.fx"

cbuffer cbPerFrame
{
    int gDirLightCount;
    DirectionalLight gDirLights[3];

    int gPointLightCount;
    PointLight gPointLights[50];

    float3 gEyePosW;
    
    float4x4 gLightView;
    float4x4 gLightProj;
    
    float gAttenuation[30];
    
    float4x4 gDecalWorldInv[50];
    float gDecalTimer[50];
    
    int gApplyPattern[50];
};

// ���� �޴� ��ä�� ������
Texture2D PositionTexture : register(t0);
Texture2D DiffuseAlbedoTexture : register(t1);
Texture2D BumpedNormalTexture : register(t2);
Texture2D EmissiveTexture : register(t3);
Texture2D MaterialTexture : register(t4);
Texture2D AdditionalTexture : register(t5);
// ���� �� ���޿�(G-Buffer�� ����� depth ������ �����ͼ� �׸���)
Texture2D gShadowDepthMapTexture;

Texture2D gDecalTexture[50];
Texture2D gDecalPositionTexture;
Texture2D gWaveTexture;

TextureCube gCubeMap;

int gDecalNum;

float gTimer;

//SamplerState samAnisotropic
//{
//    Filter = MIN_MAG_MIP_LINEAR;
//
//	//MaxAnisotropy = 4;
//
//    AddressU = CLAMP;
//    AddressV = CLAMP;
//    AddressW = CLAMP;
//};



float3 ToneMapReinhard(float3 color)
{
    float3 mappedColor = color / (color + float3(1.0, 1.0, 1.0));
    return mappedColor;
}

SamplerState samAnisotropic
{
    Filter = MIN_MAG_MIP_POINT;
    MaxAnisotropy = 16;

    AddressU = WRAP;
    AddressV = WRAP;

    MipLODBias = 0.0;
    MinLOD = 0;
    MaxLOD = 15;
};

SamplerState samNormal
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

SamplerComparisonState samShadow
{
    //Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    BorderColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

    ComparisonFunc = LESS_EQUAL;
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


float4 ToonColorRamp[2] =
{
	// Ambient light
	// 0.0f�� �ϸ� ���� ���������� ��µȴ�
    float4(0.4f, 0.4f, 0.4f, 1.0f), // ��ο� ���� ����(0.5 ��Ӱ� ���)
	float4(1.0f, 1.0f, 1.0f, 1.0f) // ���̶���Ʈ ���� ����(������ ���)
};

float ToonShade(float intensity)
{
	//// ���� ������ ������� �� ���̵� ���� ����
	//if (intensity < 0.1f) return ToonColorRamp[0];
	//else if (intensity < 0.5f) return ToonColorRamp[1];
	////else if (intensity < 0.75f) return ToonColorRamp[2];
	//else return ToonColorRamp[2];

	// ���� �������� ������ �κ��� �����Ѵ�
    if (intensity < 0.5f)
        return ToonColorRamp[0];
    else
        return ToonColorRamp[1];

}


void GetGBufferAttributes(float2 texCoord, out float3 normal, out float3 position, out float4 diffuseAlbedo, out float3 emissive, out float4 material, out float cartoon, out float2 decal, out float3 posDecal)
{
    position = PositionTexture.Sample(samAnisotropic, texCoord).xyz;

    diffuseAlbedo = DiffuseAlbedoTexture.Sample(samAnisotropic, texCoord);

    normal = BumpedNormalTexture.Sample(samAnisotropic, texCoord).xyz;
	
    emissive = EmissiveTexture.Sample(samAnisotropic, texCoord).xyz;
    
    material = MaterialTexture.Sample(samAnisotropic, texCoord);
    
    cartoon = AdditionalTexture.Sample(samAnisotropic, texCoord).x;
    
    decal = AdditionalTexture.Sample(samAnisotropic, texCoord).yz;
    
    posDecal = gDecalPositionTexture.Sample(samAnisotropic, texCoord).xyz;
}

float4 WorldToLight(float3 position)
{
    float4 worldPos = float4(position, 1.0f);
    
    float4 worldPosByLight = mul(worldPos, gLightView);
    
    worldPosByLight = mul(worldPosByLight, gLightProj);
    
    return worldPosByLight;
}

float2 ToTexcoord(float4 position)
{
    float4 ndcPos = position / position.w;
    
    float2 texCoord = float2((ndcPos.x + 1.0f) * 0.5f, (1.0f - ndcPos.y) * 0.5f);
    
    return texCoord;
}


VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    vout.Pos = float4(vin.Pos, 1.0f);
    vout.Tex = vin.Tex;
    
    return vout;
}

float4 PS(VertexOut pin, uniform bool gUseTexure, uniform bool gReflect) : SV_Target
{
    float3 normal;
    float3 position;
    float4 diffuseAlbedo;
    float3 emissive;
    float4 material;
    float cartoon;
    float2 decalInfo;
    float3 posDecal;
    
	// Specular Reflaction
    float _Glossiness;
    float4 _SpecularColor;

    GetGBufferAttributes(pin.Tex, normal, position, diffuseAlbedo, emissive, material, cartoon, decalInfo, posDecal);
       
   //if (diffuseAlbedo.a == 0.f)
   //{
   //    return float4(diffuseAlbedo.xyz, 1.0f);
   //}
    
    float3 toEye = gEyePosW - position;

    float distToEye = length(toEye);

    toEye /= distToEye;

    //// ���� �ȼ��� ������ǥ�� ������ VIEW,RPOJ����� ���� ������������ �̵���Ŵ
    //float4 worldPosByLight = WorldToLight(position);
    //
    //// ���� �ȼ��� �������������� Z���� ������
    //float vertexDepth = worldPosByLight.z / worldPosByLight.w;
    //

//    //float4 texColor = (diffuseAlbedo.a <= 0.f) ? float4(diffuseAlbedo.xyz, 1.0f) : float4(diffuseAlbedo.xyz * shadowFactor, 1.0f);
    //
    //// ������������ ������Ʈ�� ���̸� ��Ƴ��� �ؽ��Ŀ��� ���� �ȼ��� Z���� ������
    //float lightDepth = gShadowDepthMapTexture.Sample(samAnisotropic, ToTexcoord(worldPosByLight)).r;
    
   // float shadowFactor;
   // 
   // // ���� �տ� �ٸ� ��ü�� �־� LIGHTDepth�� �� �۴ٸ� �׸��ڰ� ����
   // float bias = 0.005f;
   // //if (vertexDepth > lightDepth + 0.0000125f)
   // if (vertexDepth > lightDepth + bias)
   // {
   //     shadowFactor = 0.5f;
   // }
   // else
   // {
   //     shadowFactor = 1.0f;
   // }
   // 
   // float4 texColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //diffuseAlbedo.a = 1.0f;
    
    //if (diffuseAlbedo.a <= 0.f)
    //{
    //    cartoon = 0.0f;
    //    texColor = float4(diffuseAlbedo.xyz, 1.0f);
    //}
    //else
    //{
    //    texColor = float4(diffuseAlbedo.xyz * shadowFactor, 1.0f);
    //}
    
    Material nowMat;
    nowMat.Diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
    nowMat.Ambient = nowMat.Diffuse;
    nowMat.Specular = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    //nowMat.Ambient = float4(material.x, material.x, material.x, 1.0f);
    //nowMat.Diffuse = float4(material.y, material.y, material.y, 1.0f);
    //nowMat.Specular = float4(material.z, material.z, material.z, material.w);
    
    float3 toLightDir = -gDirLights[0].Direction;

    // ���� �ȼ��� ��ġ�� ���� �� �������� �ٲ۴�
    float4 lightView = mul(float4(position, 1.0f), gLightView);
    // ���� ���� �������� ��ȯ
    float4 lightViewProj = mul(lightView, gLightProj);
    // �׸��� ���͸� �ʱ�ȭ
    float shadowFactor = 1.0f;
    // �׸��� ��ǥ�� ����Ѵ�
    float3 shadowCoord = lightViewProj.xyz / lightViewProj.w;
    // �׸��� ��ǥ�� x,y ���� �ؽ��� ��ǥ�� ��ȯ�Ѵ�
    // �̸� [-1,1]������ �ƴ� [0,1]������ �ٲ��ش�
    shadowCoord.xy = (shadowCoord * 0.5) + 0.5;
    // y��ǥ�� �����´�
    shadowCoord.y = 1.0f - shadowCoord.y;
    //// �׸��� ��ǥ�� z ���� �ȼ� ���̷� ��ȯ�Ѵ�
    
 
    
    const float dx = 1.0f / 2048.f;
    
    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };
    
    
    // �׸��� �ʿ��� �ش� �ȼ��� ���̸� ���ø��Ѵ�
    [unroll]
    for (int i = 0; i < 9; i++)
    {
        percentLit += gShadowDepthMapTexture.SampleCmpLevelZero(samShadow, shadowCoord.xy + offsets[i], shadowCoord.z).r;
    }
    
    percentLit /= 9.0f;
    //percentLit *= 0.5f;
    
    float shadowIntensity = 0.5f;
    percentLit = lerp(1.0f, percentLit, shadowIntensity);
    
    float4 texColor = float4(diffuseAlbedo.xyz, 1.0f);
    
    //texColor * percentLit;
    
	// Rim Lighting
    float rimIntensity = 0.2f;
    float rimThreshold = 0.2f;
    float rim = saturate(dot(normal, toLightDir)); // �ü� ������ �������� Rim�� ����մϴ�
    rim = smoothstep(1.0f - rimThreshold, 1.0f, rim); // �Ӱ谪�� ���� Rim�� �ε巴�� �����մϴ�
    float3 rimColor = float3(0.0f, 0.0f, 0.0f); // Rim ���� ������ �����մϴ� (�ʿ信 ���� ���� ����)
    float4 rimLighting = rim * rimIntensity * float4(rimColor, 1.0f);

	// Toon Shading
	// NdotL
    //float NdotL = dot((texColor,1.0f), toLightDir);
    float NdotL = dot(normal, toLightDir);

    float4 litColor;
    float lightIntensity;
    float4 toonColor;
    
    // global ambient
    float4 ambient = float4(0.1f, 0.1f, 0.1f, 1.0f);
    
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
       	// ���� ����
    lightIntensity = smoothstep(0, 1.f, NdotL);
        
            //float lightIntensity = NdotL > 0 ? 1 : 0;
	// ���� ������ ���� ���� ����� ���̾ �����ش� 
    toonColor = ToonShade(lightIntensity);
    //if (cartoon == 1.0f)
    //{
    //    litColor = texColor * toonColor;
    //}
    //else
    { 
        for (int i = 0; i < gDirLightCount; ++i)
        {
            float4 A, D, S;
        
            ComputeDirectionalLight(nowMat, gDirLights[i], normal, toEye,
				A, D, S);
        
            ambient += A;
            diffuse += D;
            spec += S;
        }
    

        for (int j = 0; j < gPointLightCount; ++j)
        {
            float4 A, D, S;
        
            ComputePointLight(nowMat, gPointLights[j], position, normal, toEye,
				A, D, S, gAttenuation[j]);
        
            ambient += A;
            diffuse += D;
            spec += S;
        }

        litColor = texColor * toonColor * (ambient + diffuse) + spec;
    }
    

    
	// Specular Reflection
    _Glossiness = 64.f;
    _SpecularColor = (0.0f, 0.0f, 0.0f, 1.f);
    
	//float3 halfVector = normalize(toLightDir + toEye);
    float3 halfVector = normalize(toLightDir);
    float NdotH = dot(normal, halfVector);
    float specularIntensity = pow(NdotH * lightIntensity, _Glossiness * _Glossiness);
    float specularIntensitySmooth = smoothstep(0.005, 0.01, specularIntensity);
    float4 specular = specularIntensitySmooth * _SpecularColor;

	// Emissive
    //float4 emissiveColor = float4(emissive, 1.0f);
    //litColor += emissiveColor;

	// OutLine
    float outlineThreshold = 0.99f; // �ܰ��� ���� �Ӱ谪
    float4 outlineColor = float4(0.0f, 0.0f, 0.0f, 1.0f); // �ܰ��� ����
    float outlineFactor = saturate(1.0f - dot(normal, toLightDir)); // ��ְ� dirLight ���� ������ ���� �ܰ��� ���� ����
    float4 outline = outlineFactor > outlineThreshold ? outlineColor : float4(0.0f, 0.0f, 0.0f, 0.0f);

	
	// ���� ��ü�� ����� �ܰ����� ����
    float4 finalColor = (litColor + outline + specular + rimLighting);
    
    float3 toneMappedColor = ToneMapReinhard(finalColor.xyz);
    
    if (decalInfo.x == 1)
    {
        for (int i = 0; i < gDecalNum; i++)
        {        
            float4 toDecal = mul(float4(posDecal, 1.0f), gDecalWorldInv[i]);
    
    // ��Į �ڽ� ���� ��ǥ�� �ؽ�ó ��ǥ�� ��ȯ
            float2 decalUV = (toDecal.xz + 0.5f);
            
             // ������ �ؽ�ó ��ǥ�� ���
            //float2 noiseUV = decalUV * 10.0f + 0.3f * 0.1f; // �ð��� ���� ��ȭ�ϴ� ��ǥ
            //float3 noiseSample = gWaveTexture.Sample(samNormal, noiseUV).xyz;
                        
            //decalUV += (noiseSample.xy - 0.5f) * 0.05f;
            
            float3 inDecal = 0.5f - abs(toDecal.xyz);
            
            float alpha = saturate(1.0f - gDecalTimer[i]);
    
    // ��Į �ڽ� ���ο� �ִ��� Ȯ��
            if (inDecal.x >= 0 && inDecal.y >= 0 && inDecal.z >= 0)
            {
                float4 texSample = gDecalTexture[i].Sample(samAnisotropic, decalUV);
                
                if (gApplyPattern[i])
                {
                    texSample *= 0.5f;
                    
                    float2 waveUv = float2(position.x - (100), abs(position.z - 110));
                    waveUv.x *= 0.005f;
                    waveUv.y *= 0.0043478f;
                
                    waveUv *= 4.0f;
                
                    float4 waveSample = gWaveTexture.Sample(samNormal, waveUv);
                
                    waveSample.r += 0.5f;
                    texSample += waveSample * 0.5f;
                }

                if (texSample.a > 0.0f)
                {
                    if (toneMappedColor.r != 0)
                    {
                        toneMappedColor.r = lerp(toneMappedColor.r, texSample.r, texSample.a * alpha * 0.8f);
                        toneMappedColor.g = lerp(toneMappedColor.g, texSample.g, texSample.a * alpha * 0.8f);
                        toneMappedColor.b = lerp(toneMappedColor.b, texSample.b, texSample.a * alpha * 0.8f);
                    }
                }
            }
        }
    }
    float4 emissiveColor = float4(emissive, 0.0f);
    return (float4(toneMappedColor, 1.0f) + emissiveColor) * percentLit;
    
    //return finalColor;
   
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