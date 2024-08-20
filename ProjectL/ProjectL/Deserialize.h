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



class Deserialize
{
public:
	Deserialize();
	~Deserialize();

public:
	void Initialize(std::string& deserialize);

	void CreateData(std::string& deserialize);

	void SetChangeSceneData();

private:
	// scene�� ������ ������
	bool _scene1First;
	bool _scene2First;
	bool _scene3First;
	bool _scene4First;
	bool _scene5First;

	std::filesystem::path _executablePath;
};

