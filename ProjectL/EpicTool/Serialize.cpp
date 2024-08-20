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

	// 오브젝트의 갯수 만큼 반복 할 것
	for (const auto gameObject : gameObjectlist)
	{
		 // 보스나 플레이어면 넘어감
		if (gameObject->GetComponent<KunrealEngine::Player>() != nullptr)
		{
			continue;
		}

		if (gameObject->GetTag() == "PlayerSub") // 보스나 플레이어에서 내부적으로 만든거면 넘어감
		{
			continue;
		}

		if (gameObject->GetTag() == "BossSub") // 보스나 플레이어에서 내부적으로 만든거면 넘어감
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

			// 부모 오브젝트 설정
			if (gameObject->GetParent() != nullptr)
			{
				pod.parent["ParentName"] = gameObject->GetParent()->GetObjectName();
			}

			// 커스텀 컴포넌트 (특수한 경우의 컴포넌트)
			if ((gameObject->GetComponent<KunrealEngine::Player>()) != NULL)  
			{
				pod.customComponent["Player"] = true;
			}

			if ((gameObject->GetComponent<KunrealEngine::Kamen>()) != NULL)  
			{
				pod.customComponent["Kamen"] = true;
			}

			if ((gameObject->GetComponent<KunrealEngine::Camera>()) != NULL)  // 그냥 addCompoent만 해주면 되는가?
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
					pod.image["ImageName"] = gameObject->GetComponent<KunrealEngine::ImageRenderer>()->GetImageName();  // 현재 가지고 있는 이미지를 get 할 수 없음

					if (gameObject->GetComponent<KunrealEngine::ImageRenderer>()->GetImageStatus() == true)
					{
						pod.image["ImageStatus"] = "True";
					}
					else if (gameObject->GetComponent<KunrealEngine::ImageRenderer>()->GetImageStatus() == false)
					{
						pod.image["ImageStatus"] = "False";
					}
				}


				// 콜라이더
				if ((gameObject->GetComponent<KunrealEngine::BoxCollider>()) != NULL)
				{
					pod.collider["Offset_x"] = gameObject->GetComponent<KunrealEngine::BoxCollider>()->GetOffset().x;
					pod.collider["Offset_y"] = gameObject->GetComponent<KunrealEngine::BoxCollider>()->GetOffset().y;
					pod.collider["Offset_z"] = gameObject->GetComponent<KunrealEngine::BoxCollider>()->GetOffset().z;

					pod.collider["BoxSize_x"] = gameObject->GetComponent<KunrealEngine::BoxCollider>()->GetColliderScale().x;
					pod.collider["BoxSize_y"] = gameObject->GetComponent<KunrealEngine::BoxCollider>()->GetColliderScale().y;
					pod.collider["BoxSize_z"] = gameObject->GetComponent<KunrealEngine::BoxCollider>()->GetColliderScale().z;
				}

				// 파티클
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


				if ((gameObject->GetComponent<KunrealEngine::SoundPlayer>()) != NULL)  // 그냥 addCompoent만 해주면 되는가?
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

			PreEncoding(serializedData);  // pod의 정보를 json으로 저장

			newJsonData.push_back(serializedData); //저장한 정보를 추가

			// 필드를 초기화
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
	
	// json 객체를 파일로 출력
	std::ofstream outputFile(filePath);
	if (outputFile.is_open())
	{
		outputFile << std::setw(4) << newJsonData << std::endl; // setw(4)는 pretty print를 위한 것
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
	// 포인터 저장 부분은 미구현 => 이해부족
	//EpicTool::FieldPostEncoding<boost::pfr::tuple_size_v<Field>>(pod, data["POD"]);

	return true;
}
