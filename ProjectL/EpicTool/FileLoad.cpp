#include "FileLoad.h"

#include "KunrealAPI.h"
#include "SceneManager.h"

EpicTool::FileLoad::FileLoad()
{

}

EpicTool::FileLoad::~FileLoad()
{

}
/// <summary>
/// 리플랙션으로 대부분 폐기 예정
/// 2024.01.08
/// 
/// 컴포넌트 기반이라 디시리얼라이즈가 필요하다고 판단함
/// 2024.03.04
/// </summary>
void EpicTool::FileLoad::Initialize()  // 아래 부분은 이후에 반복적으로 호출 될 수 있는 부분이기에 분리할 수도 있다 , 
{
    //json파일 중에 앞서 저장해둔 파일을 읽어옴
   std::ifstream inputFile("output.json");
   

				//	auto meshRenderer = jsonItem["MeshData"].find("MeshRenderer");
				//	auto textureRenderer = jsonItem["MeshData"].find("TextureRenderer");

				//	if (meshRenderer != jsonItem["MeshData"].end() && !jsonItem["MeshData"]["MeshRenderer"].empty())
    //                {
				//		object->AddComponent<KunrealEngine::MeshRenderer>();
				//		KunrealEngine::MeshRenderer* mesh = object->GetComponent<KunrealEngine::MeshRenderer>();

				//		if (!jsonItem["MeshData"]["MeshRenderer"][0]["Mesh"].empty())
    //                    {
				//			if (!mesh->GetMeshStatus())
    //                        {
				//				std::string meshName = jsonItem["MeshData"]["MeshRenderer"][0]["Mesh"];
				//				mesh->SetMeshObject(meshName.c_str());

				//				if (jsonItem["MeshData"]["MeshRenderer"].size() > 1)
    //                            {
				//					mesh->SetActive(jsonItem["MeshData"]["MeshRenderer"][1]["RenderingState"]);
				//				}
				//			}
				//			else
    //                        {
				//				std::string meshName = jsonItem["MeshData"]["MeshRenderer"][0]["Mesh"];
				//				mesh->ChangeMeshObject(meshName.c_str());

				//				if (jsonItem["MeshData"]["MeshRenderer"].size() > 1)
    //                            {
				//					mesh->SetActive(jsonItem["MeshData"]["MeshRenderer"][1]["RenderingState"]);
				//				}
				//			}
				//		}
				//	}

				//	if (textureRenderer != jsonItem["MeshData"].end() && !jsonItem["MeshData"]["TextureRenderer"].empty())
    //                {
				//		object->AddComponent<KunrealEngine::MeshRenderer>();
				//		KunrealEngine::MeshRenderer* mesh = object->GetComponent<KunrealEngine::MeshRenderer>();

    //                    if (mesh->GetMeshStatus())
    //                    {
				//			if (!jsonItem["MeshData"]["TextureRenderer"][0]["Diffuse"].empty())
				//			{
				//				std::string diffuseTexture = jsonItem["MeshData"]["TextureRenderer"][0]["Diffuse"];
				//				mesh->SetDiffuseTexture(diffuseTexture.c_str());
				//			}

				//			if (jsonItem["MeshData"]["TextureRenderer"].size() > 1 && !jsonItem["MeshData"]["TextureRenderer"][1]["Normal"].empty())
				//			{
				//				std::string normalTexture = jsonItem["MeshData"]["TextureRenderer"][1]["Normal"];
				//				mesh->SetNormalTexture(normalTexture.c_str());
				//			}
				//		}
				//	}


    //        		objectIndex.emplace_back(object);
    //        	}
    //        }
    //    }
    //}
}

void EpicTool::FileLoad::Update()
{

}
