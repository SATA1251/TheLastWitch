#include "Serialize.h"
#include "KunrealAPI.h"
#include <nfd.h>
#include <fstream>

EpicTool::Serialize::Serialize()
{

}

EpicTool::Serialize::~Serialize()
{

}

void EpicTool::Serialize::Initialize()
{

}

void EpicTool::Serialize::SaveFile(const std::string& filePath)
{
	nlohmann::json jsonData;
	nlohmann::json newJsonData;

	std::vector<KunrealEngine::GameObject*> gameObjectlist = KunrealEngine::GetCurrentScene()->
		GetObjectList();

	if (std::ifstream(filePath).good())
	{
		std::ifstream inputFile(filePath);
		inputFile >> jsonData;
		inputFile.close();
	}

	// ������Ʈ�� ���� ��ŭ �ݺ� �� ��
	for (const auto gameObject : gameObjectlist)
	{
		 // ������ �÷��̾�� �Ѿ
		if (gameObject->GetComponent<KunrealEngine::Player>() != nullptr)
		{
			continue;
		}

		if (gameObject->GetTag() == "PlayerSub") // ������ �÷��̾�� ���������� ����Ÿ� �Ѿ
		{
			continue;
		}

		if (gameObject->GetTag() == "BossSub") // ������ �÷��̾�� ���������� ����Ÿ� �Ѿ
		{
			continue;
		}

		if (gameObject->GetObjectName() != "MainCamera" )
		{		
			//pod.animationFrame = 0.0f;
			nlohmann::json saveJson;
			std::string objectName = gameObject->GetObjectName();

			pod.name = objectName;

			pod.transform["position_x"] = gameObject->GetComponent<KunrealEngine::Transform>()->GetPosition().x;
			pod.transform["position_y"] = gameObject->GetComponent<KunrealEngine::Transform>()->GetPosition().y;
			pod.transform["position_z"] = gameObject->GetComponent<KunrealEngine::Transform>()->GetPosition().z;

			pod.transform["rotation_x"] = gameObject->GetComponent<KunrealEngine::Transform>()->GetRotation().x;
			pod.transform["rotation_y"] = gameObject->GetComponent<KunrealEngine::Transform>()->GetRotation().y;
			pod.transform["rotation_z"] = gameObject->GetComponent<KunrealEngine::Transform>()->GetRotation().z;

			pod.transform["scale_x"] = gameObject->GetComponent<KunrealEngine::Transform>()->GetScale().x;
			pod.transform["scale_y"] = gameObject->GetComponent<KunrealEngine::Transform>()->GetScale().y;
			pod.transform["scale_z"] = gameObject->GetComponent<KunrealEngine::Transform>()->GetScale().z;

			// �θ� ������Ʈ ����
			if (gameObject->GetParent() != nullptr)
			{
				pod.parent["ParentName"] = gameObject->GetParent()->GetObjectName();
			}

			// Ŀ���� ������Ʈ (Ư���� ����� ������Ʈ)
			if ((gameObject->GetComponent<KunrealEngine::Player>()) != NULL)  
			{
				pod.customComponent["Player"] = true;
			}

			if ((gameObject->GetComponent<KunrealEngine::Kamen>()) != NULL)  
			{
				pod.customComponent["Kamen"] = true;
			}

			if ((gameObject->GetComponent<KunrealEngine::Camera>()) != NULL)  // �׳� addCompoent�� ���ָ� �Ǵ°�?
			{
				pod.customComponent["Camera"] = true;
				pod.transform["rotation_x"] = 0.0;
				pod.transform["rotation_y"] = 0.0;
				pod.transform["rotation_z"] = 0.0;
			}


			if (gameObject->GetParent() != nullptr)
			{
				if (gameObject->GetParent()->GetComponent<KunrealEngine::Kamen>() != nullptr)
				{
					continue;
				}
			}

			if (gameObject->GetComponent<KunrealEngine::Player>() == NULL && gameObject->GetComponent<KunrealEngine::Kamen>() == NULL)
			{
				if ((gameObject->GetComponent<KunrealEngine::MeshRenderer>()) != NULL)
				{
					std::vector<std::string> meshDiffuseJson;
					std::vector<std::string> meshNormalJson;

					for (const auto& diffuseJson : gameObject->GetComponent<KunrealEngine::MeshRenderer>()->GetTextures())
					{
						meshDiffuseJson.push_back(diffuseJson);
					}
					for (const auto& NormalJson : gameObject->GetComponent<KunrealEngine::MeshRenderer>()->GetNormals())
					{
						meshNormalJson.push_back(NormalJson);
					}

					pod.meshRenderingState = gameObject->GetComponent<KunrealEngine::MeshRenderer>()->GetRenderingState();

					pod.meshRenderer["Mesh"].push_back(gameObject->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshName());
					pod.meshRenderer["Diffuse"] = meshDiffuseJson;
					pod.meshRenderer["Normal"] = meshNormalJson;

					//pod.animationFrame = gameObject->GetComponent<KunrealEngine::Animator>()->GetCurrentFrame();

					if (gameObject->GetComponent<KunrealEngine::MeshRenderer>()->GetAlpha() == 0)
					{
						pod.customComponent["Invisible"] = true;
					}
				}

				if ((gameObject->GetComponent<KunrealEngine::Light>()) != NULL)
				{

					if (gameObject->GetComponent<KunrealEngine::Light>()->GetLightType() == KunrealEngine::LightType::DirectionalLight)
					{
						pod.lightType["LightType"] = "DirectionalLight";
						    
						pod.light["Ambient_x"] = gameObject->GetComponent<KunrealEngine::Light>()->GetAmbient().x;
						pod.light["Ambient_y"] = gameObject->GetComponent<KunrealEngine::Light>()->GetAmbient().y;
						pod.light["Ambient_z"] = gameObject->GetComponent<KunrealEngine::Light>()->GetAmbient().z;
						pod.light["Ambient_w"] = gameObject->GetComponent<KunrealEngine::Light>()->GetAmbient().w;

						pod.light["Diffuse_x"] = gameObject->GetComponent<KunrealEngine::Light>()->GetDiffuse().x;
						pod.light["Diffuse_y"] = gameObject->GetComponent<KunrealEngine::Light>()->GetDiffuse().y;
						pod.light["Diffuse_z"] = gameObject->GetComponent<KunrealEngine::Light>()->GetDiffuse().z;
						pod.light["Diffuse_w"] = gameObject->GetComponent<KunrealEngine::Light>()->GetDiffuse().w;

						pod.light["Direction_x"] = gameObject->GetComponent<KunrealEngine::Light>()->GetDirection().x;
						pod.light["Direction_y"] = gameObject->GetComponent<KunrealEngine::Light>()->GetDirection().y;
						pod.light["Direction_z"] = gameObject->GetComponent<KunrealEngine::Light>()->GetDirection().z;

						pod.light["Specular_x"] = gameObject->GetComponent<KunrealEngine::Light>()->GetSpecular().x;
						pod.light["Specular_y"] = gameObject->GetComponent<KunrealEngine::Light>()->GetSpecular().y;
						pod.light["Specular_z"] = gameObject->GetComponent<KunrealEngine::Light>()->GetSpecular().z;
						pod.light["Specular_w"] = gameObject->GetComponent<KunrealEngine::Light>()->GetSpecular().w;
					}
					else if (gameObject->GetComponent<KunrealEngine::Light>()->GetLightType() == KunrealEngine::LightType::PointLight)
					{
						pod.lightType["LightType"] = "PointLight";

						pod.light["Ambient_x"] = gameObject->GetComponent<KunrealEngine::Light>()->GetAmbient().x;
						pod.light["Ambient_y"] = gameObject->GetComponent<KunrealEngine::Light>()->GetAmbient().y;
						pod.light["Ambient_z"] = gameObject->GetComponent<KunrealEngine::Light>()->GetAmbient().z;
						pod.light["Ambient_w"] = gameObject->GetComponent<KunrealEngine::Light>()->GetAmbient().w;

						pod.light["Diffuse_x"] = gameObject->GetComponent<KunrealEngine::Light>()->GetDiffuse().x;
						pod.light["Diffuse_y"] = gameObject->GetComponent<KunrealEngine::Light>()->GetDiffuse().y;
						pod.light["Diffuse_z"] = gameObject->GetComponent<KunrealEngine::Light>()->GetDiffuse().z;
						pod.light["Diffuse_w"] = gameObject->GetComponent<KunrealEngine::Light>()->GetDiffuse().w;

						pod.light["Specular_x"] = gameObject->GetComponent<KunrealEngine::Light>()->GetSpecular().x;
						pod.light["Specular_y"] = gameObject->GetComponent<KunrealEngine::Light>()->GetSpecular().y;
						pod.light["Specular_z"] = gameObject->GetComponent<KunrealEngine::Light>()->GetSpecular().z;
						pod.light["Specular_w"] = gameObject->GetComponent<KunrealEngine::Light>()->GetSpecular().w;

						pod.light["Range"] = gameObject->GetComponent<KunrealEngine::Light>()->GetPointRange();
					}
				}


				if ((gameObject->GetComponent<KunrealEngine::ImageRenderer>()) != NULL)
				{
					pod.image["ImageName"] = gameObject->GetComponent<KunrealEngine::ImageRenderer>()->GetImageName();  // ���� ������ �ִ� �̹����� get �� �� ����

					if (gameObject->GetComponent<KunrealEngine::ImageRenderer>()->GetImageStatus() == true)
					{
						pod.image["ImageStatus"] = "True";
					}
					else if (gameObject->GetComponent<KunrealEngine::ImageRenderer>()->GetImageStatus() == false)
					{
						pod.image["ImageStatus"] = "False";
					}
				}


				// �ݶ��̴�
				if ((gameObject->GetComponent<KunrealEngine::BoxCollider>()) != NULL)
				{
					pod.collider["Offset_x"] = gameObject->GetComponent<KunrealEngine::BoxCollider>()->GetOffset().x;
					pod.collider["Offset_y"] = gameObject->GetComponent<KunrealEngine::BoxCollider>()->GetOffset().y;
					pod.collider["Offset_z"] = gameObject->GetComponent<KunrealEngine::BoxCollider>()->GetOffset().z;

					pod.collider["BoxSize_x"] = gameObject->GetComponent<KunrealEngine::BoxCollider>()->GetColliderScale().x;
					pod.collider["BoxSize_y"] = gameObject->GetComponent<KunrealEngine::BoxCollider>()->GetColliderScale().y;
					pod.collider["BoxSize_z"] = gameObject->GetComponent<KunrealEngine::BoxCollider>()->GetColliderScale().z;
				}

				// ��ƼŬ
				if ((gameObject->GetComponent<KunrealEngine::Particle>()) != NULL)
				{
					pod.particle["Velocity"] = gameObject->GetComponent<KunrealEngine::Particle>()->GetVelocity();

					if (gameObject->GetComponent<KunrealEngine::Particle>()->GetRandomState() == true)
					{
						pod.particle["Random"] = 1.0f;
					}
					else
					{
						pod.particle["Random"] = 0.0f;
					}

					pod.particle["FadeoutTime"] = gameObject->GetComponent<KunrealEngine::Particle>()->GetFadeOutTime();
					pod.particle["LifeTime"] = gameObject->GetComponent<KunrealEngine::Particle>()->GetLifeTime();

					pod.particle["Color_X"] = gameObject->GetComponent<KunrealEngine::Particle>()->GetColor().x;
					pod.particle["Color_Y"] = gameObject->GetComponent<KunrealEngine::Particle>()->GetColor().y;
					pod.particle["Color_Z"] = gameObject->GetComponent<KunrealEngine::Particle>()->GetColor().z;

					pod.particle["Direction_X"] = gameObject->GetComponent<KunrealEngine::Particle>()->GetDirection().x;
					pod.particle["Direction_Y"] = gameObject->GetComponent<KunrealEngine::Particle>()->GetDirection().y;
					pod.particle["Direction_Z"] = gameObject->GetComponent<KunrealEngine::Particle>()->GetDirection().z;
						
					pod.particle["Size_X"] = gameObject->GetComponent<KunrealEngine::Particle>()->GetSize().x;
					pod.particle["Size_Y"] = gameObject->GetComponent<KunrealEngine::Particle>()->GetSize().y;
				}


				if ((gameObject->GetComponent<KunrealEngine::SoundPlayer>()) != NULL)  // �׳� addCompoent�� ���ָ� �Ǵ°�?
				{
					for (int i = 0; i < gameObject->GetComponent<KunrealEngine::SoundPlayer>()->_soundList.size(); ++i)
					{
						pod.sound_Name.push_back(gameObject->GetComponent<KunrealEngine::SoundPlayer>()->_soundList[i]._soundPath);
						pod.sound_Is3D.push_back(gameObject->GetComponent<KunrealEngine::SoundPlayer>()->_soundList[i]._is3DSound);
						pod.sound_IsLoop.push_back(gameObject->GetComponent<KunrealEngine::SoundPlayer>()->_soundList[i]._isLoop);
						pod.sound_Volume.push_back(gameObject->GetComponent<KunrealEngine::SoundPlayer>()->_soundList[i]._volume);
					}
				}
			}

			nlohmann::json serializedData;

			PreEncoding(serializedData);  // pod�� ������ json���� ����

			newJsonData.push_back(serializedData); //������ ������ �߰�

			// �ʵ带 �ʱ�ȭ
			pod.transform.clear();
			pod.lightType.clear();
			pod.light.clear();
			pod.meshRenderer.clear();
			pod.image.clear();
			pod.customComponent.clear();
			pod.collider.clear();
			pod.sound_Name.clear();
			pod.sound_Is3D.clear();
			pod.sound_IsLoop.clear();
			pod.sound_Volume.clear();
			pod.parent.clear();
		}
	}
	
	// json ��ü�� ���Ϸ� ���
	std::ofstream outputFile(filePath);
	if (outputFile.is_open())
	{
		outputFile << std::setw(4) << newJsonData << std::endl; // setw(4)�� pretty print�� ���� ��
		outputFile.close();
	}

	
}
  
bool EpicTool::Serialize::PreEncoding(json& data) const
{
	EpicTool::FieldPreEncoding<boost::pfr::tuple_size_v<Field>>(pod, data["POD"]);

	return true;
}

bool EpicTool::Serialize::PostEncoding(json& data) const
{
	// ������ ���� �κ��� �̱��� => ���غ���
	//EpicTool::FieldPostEncoding<boost::pfr::tuple_size_v<Field>>(pod, data["POD"]);

	return true;
}
