#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>

/// <summary>
/// �ø���������� json�� ���� �����͸� ������
/// 2024.03.11
/// </summary>


namespace KunrealEngine
{
	class Transform;
	class MeshRenderer;
	class Light;
	class ImageRenderer;
	class BoxCollider;
	class SoundPlayer;
	class CustomComponent;
}


namespace EpicTool
{

	class Deserialize
	{
	public:
		Deserialize();
		~Deserialize();

	public:
		void Initialize(std::string& deserialize);
	};
}
