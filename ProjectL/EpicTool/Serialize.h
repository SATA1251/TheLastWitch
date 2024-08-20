#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <variant>

#include <boost/pfr.hpp>
#include <boost/pfr/core.hpp>
#include <boost/pfr/core_name.hpp>
#include <nlohmann/json.hpp>
#include "Storable.h"

using json = nlohmann::json;

namespace KunrealEngine
{
	class Transform;
	class MeshRenderer;
	class Component;
	class Light;
	class GameObject;
}

struct Field
{
	std::string name;

	std::map<std::string, float> transform;

	// mesh
	std::map<std::string, std::vector< std::string>> meshRenderer;

	bool meshRenderingState;

	float animationFrame;

	// light
	std::map<std::string, std::string> lightType;

	std::map<std::string, float> light;

	// image
	std::map<std::string, std::string> image;
	       
	// boxCollider
	std::map<std::string, float> collider;

	// particle
	std::map<std::string, float> particle;

	// SoundPlayer
	std::vector<std::string> sound_Name;
	std::vector<bool> sound_Is3D;
	std::vector<bool> sound_IsLoop;
	std::vector<int> sound_Volume;

	// 부모를 체크
	std::map<std::string, std::string> parent;

	// 역할에 따른 고유 컴포넌트
	std::map<std::string , bool> customComponent;


	TO_JSON(Field)
};

namespace EpicTool
{

	class Serialize : public Storable
	{
	public:
		Serialize();
		~Serialize();

	public:


		void Initialize();

		void SaveFile(const std::string& filePath);

		Field pod;

	protected:
		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;

	private:
		std::vector<KunrealEngine::GameObject*> _gameObjectlist;
	
		
	};
}