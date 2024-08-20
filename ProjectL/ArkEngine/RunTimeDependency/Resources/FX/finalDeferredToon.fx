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

// 빛을 받는 객채의 포지션
Texture2D PositionTexture : register(t0);
Texture2D DiffuseAlbedoTexture : register(t1);
Texture2D BumpedNormalTexture : register(t2);
Texture2D EmissiveTexture : register(t3);
Texture2D MaterialTexture : register(t4);
Texture2D AdditionalTexture : register(t5);
// 깊이 값 전달용(G-Buffer에 저장된 depth 정보를 가져와서 그림자)
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
	// 0.0f로 하면 완전 검정색으로 출력된다
    float4(0.4f, 0.4f, 0.4f, 1.0f), // 어두운 영역 색상(0.5 어둡게 출력)
	float4(1.0f, 1.0f, 1.0f, 1.0f) // 하이라이트 영역 색상(원색을 출력)
};

float ToonShade(float intensity)
{
	//// 색상 런프를 기반으로 툰 쉐이딩 색상 결정
	//if (intensity < 0.1f) return ToonColorRamp[0];
	//else if (intensity < 0.5f) return ToonColorRamp[1];
	////else if (intensity < 0.75f) return ToonColorRamp[2];
	//else return ToonColorRamp[2];

	// 빛의 강함으로 음영질 부분을 구분한다
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

    //// 현재 픽셀의 월드좌표에 광원의 VIEW,RPOJ행렬을 곱해 광원시점으로 이동시킴
    //float4 worldPosByLight = WorldToLight(position);
    //
    //// 현재 픽셀의 광원시점에서의 Z값을 가져옴
    //float vertexDepth = worldPosByLight.z / worldPosByLight.w;
    //

//    //float4 texColor = (diffuseAlbedo.a <= 0.f) ? float4(diffuseAlbedo.xyz, 1.0f) : float4(diffuseAlbedo.xyz * shadowFactor, 1.0f);
    //
    //// 광원시점에서 오브젝트의 깊이를 담아놓은 텍스쳐에서 현재 픽셀의 Z값을 가져옴
    //float lightDepth = gShadowDepthMapTexture.Sample(samAnisotropic, ToTexcoord(worldPosByLight)).r;
    
   // float shadowFactor;
   // 
   // // 본인 앞에 다른 물체가 있어 LIGHTDepth가 더 작다면 그림자가 생김
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

    // 현재 픽셀의 위치를 광원 뷰 공간으로 바꾼다
    float4 lightView = mul(float4(position, 1.0f), gLightView);
    // 광원 투영 공간으로 변환
    float4 lightViewProj = mul(lightView, gLightProj);
    // 그림자 팩터를 초기화
    float shadowFactor = 1.0f;
    // 그림자 좌표를 계산한다
    float3 shadowCoord = lightViewProj.xyz / lightViewProj.w;
    // 그림자 좌표의 x,y 값을 텍스쳐 좌표로 변환한다
    // 이를 [-1,1]범위가 아닌 [0,1]범위로 바꿔준다
    shadowCoord.xy = (shadowCoord * 0.5) + 0.5;
    // y좌표를 뒤집는다
    shadowCoord.y = 1.0f - shadowCoord.y;
    //// 그림자 좌표의 z 값을 픽셀 깊이로 변환한다
    
 
    
    const float dx = 1.0f / 2048.f;
    
    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };
    
    
    // 그림자 맵에서 해당 픽셀의 깊이를 샘플링한다
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
    float rim = saturate(dot(normal, toLightDir)); // 시선 방향을 기준으로 Rim을 계산합니다
    rim = smoothstep(1.0f - rimThreshold, 1.0f, rim); // 임계값에 따라 Rim을 부드럽게 보정합니다
    float3 rimColor = float3(0.0f, 0.0f, 0.0f); // Rim 빛의 색상을 지정합니다 (필요에 따라 조절 가능)
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
    
       	// 빛의 강도
    lightIntensity = smoothstep(0, 1.f, NdotL);
        
            //float lightIntensity = NdotL > 0 ? 1 : 0;
	// 빛의 강도에 따라 색상 출력의 레이어를 정해준다 
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
    float outlineThreshold = 0.99f; // 외곽선 감지 임계값
    float4 outlineColor = float4(0.0f, 0.0f, 0.0f, 1.0f); // 외곽선 색상
    float outlineFactor = saturate(1.0f - dot(normal, toLightDir)); // 노멀과 dirLight 간의 각도에 따라 외곽선 감도 결정
    float4 outline = outlineFactor > outlineThreshold ? outlineColor : float4(0.0f, 0.0f, 0.0f, 0.0f);

	
	// 원래 물체의 색상과 외곽선을 더함
    float4 finalColor = (litColor + outline + specular + rimLighting);
    
    float3 toneMappedColor = ToneMapReinhard(finalColor.xyz);
    
    if (decalInfo.x == 1)
    {
        for (int i = 0; i < gDecalNum; i++)
        {        
            float4 toDecal = mul(float4(posDecal, 1.0f), gDecalWorldInv[i]);
    
    // 데칼 박스 내부 좌표를 텍스처 좌표로 변환
            float2 decalUV = (toDecal.xz + 0.5f);
            
             // 노이즈 텍스처 좌표를 계산
            //float2 noiseUV = decalUV * 10.0f + 0.3f * 0.1f; // 시간에 따라 변화하는 좌표
            //float3 noiseSample = gWaveTexture.Sample(samNormal, noiseUV).xyz;
                        
            //decalUV += (noiseSample.xy - 0.5f) * 0.05f;
            
            float3 inDecal = 0.5f - abs(toDecal.xyz);
            
            float alpha = saturate(1.0f - gDecalTimer[i]);
    
    // 데칼 박스 내부에 있는지 확인
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