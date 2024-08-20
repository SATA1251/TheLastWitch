#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include "../FbxLoader/types.h"

struct ModelKeyframeData
{
	float time = 0.f;
	// 이부분 vec3 유심히 봐볼것
	Vec3 scale = { 0,0,0 };
	Quaternion rotation = { };
	Vec3 translation = { 0,0,0 };
};

struct ModelKeyframe
{
	std::wstring boneName;
	std::vector<ModelKeyframeData> transforms;
};

struct ModelAnimation
{

	std::shared_ptr<ModelKeyframe> GetKeyframe(const std::wstring& name);

	std::string name;
	float duration = 0.f;
	float frameRate = 0.f;
	unsigned int frameCount = 0;
	
	std::unordered_map<std::wstring, std::shared_ptr<ModelKeyframe>> keyframes;
};

