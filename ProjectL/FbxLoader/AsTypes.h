#pragma once
#include "Types.h"
#include "VertexDatas.h"
//#include "../ArkEngine/VertexData.h"
//#include "../ArkEngine/Types.h"

//#ifdef ARK_EXPORTS
//#define ARK_EXPORT __declspec(dllexport)
//#else
//#define ARK_EXPORT __declspec(dllimport)
//#endif

namespace ArkEngine
{
	using VertexType = VertexTextureNormalTangentBlendData;

	struct asBone
	{
		std::string name = "";
		int index = -1;
		int parent = -1;
		Matrix transform = {};
		

		aiMatrix4x4 boneOffsetTM = {};
	};

	struct asMesh
	{
		std::string name = "";
		aiMesh* mesh = nullptr;
		std::vector<VertexType> vertices = {};
		std::vector<unsigned int> indices = {};

		int boneIndex = 0;
		std::string materialName = "";
	};

	struct asMaterial
	{
		std::string name = "";
		Color ambient = { 0,0,0,0 };
		Color diffuse = { 0,0,0,0 };
		Color specular = { 0,0,0,0 };
		Color emissive = { 0,0,0,0 };
		std::string diffuseFile = "";
		std::string specularFile = "";
		std::string normalFile = "";
		std::string emissiveFile = "";
	};

	// Animation
	struct asBlendWeight
	{
		DirectX::XMUINT4 indices = DirectX::XMUINT4(0, 0, 0, 0);
		// weight값을 vec4로 바꿔야 할듯
		Vec3 weights = Vec3(0, 0, 0);

		void Set(unsigned int index, unsigned int bonsIndex, float weight)
		{
			//float i = (float)bonsIndex;
			unsigned int i = bonsIndex;
			float w = weight;

			switch (index)
			{
				case 0: indices.x = i; weights.x = w; break;
				case 1: indices.y = i; weights.y = w; break;
				case 2: indices.z = i; weights.z = w; break;
					//case 3: indices.w = i; weights.w = w; break;
			}
		}
	};

	// 정점마다 -> (관절번호, 가중치)
	struct asBoneWeights
	{
		// 가중치가 높은 것들을 앞으로 놓기 위해
		void AddWeights(unsigned int boneIndex, float weight)
		{
			if (weight <= 0.0f)
				return;

			auto findIt = std::find_if(boneWeights.begin(), boneWeights.end(),
				[weight](const Pair& p) {return weight > p.second; });

			boneWeights.insert(findIt, Pair(boneIndex, weight));
		}

		asBlendWeight GetBlendWeights()
		{
			asBlendWeight blendWeights;

			for (unsigned int i = 0; i < boneWeights.size(); i++)
			{
				if (i >= 4)
					break;

				blendWeights.Set(i, boneWeights[i].first, boneWeights[i].second);
			}

			return blendWeights;
		}


		// 가중치의 비율의 합을 1로 보정해 준다
		void Normalize()
		{
			if (boneWeights.size() >= 4)
				boneWeights.resize(4);

			float totalWeight = 0.f;
			for (const auto& item : boneWeights)
				totalWeight += item.second;

			float scale = 1.f / totalWeight;
			for (auto& item : boneWeights)
				item.second *= scale;
		}

		using Pair = std::pair<int, float>;
		std::vector<Pair> boneWeights;

	};

	/// <summary>
	/// Animation
	/// </summary>
	struct asKeyFrameData
	{
		float time = 0.f;

		// SimpleMath
		Vector3 scale = {0,0,0};
		Quaternion rotation = {};
		Vector3 translation = {0,0,0};
	};

	struct asKeyFrame
	{
		std::string boneName;
		std::vector<asKeyFrameData> transforms;
	};

	struct asAnimation
	{
		std::string name = "";
		unsigned int frameCount = 0;
		float frameRate  = 0.f;
		float duration = 0.f;

		// 2차원 배열, vector 안에 vector
		std::vector<std::shared_ptr<asKeyFrame>> keyFrame = {};
	};

	// Cache
	struct asAnimationNode
	{
		aiString name;
		std::vector<asKeyFrameData> keyframe;
	};
}