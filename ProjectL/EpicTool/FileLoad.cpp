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
/// ���÷������� ��κ� ��� ����
/// 2024.01.08
/// 
/// ������Ʈ ����̶� ��ø������� �ʿ��ϴٰ� �Ǵ���
/// 2024.03.04
/// </summary>
void EpicTool::FileLoad::Initialize()  // �Ʒ� �κ��� ���Ŀ� �ݺ������� ȣ�� �� �� �ִ� �κ��̱⿡ �и��� ���� �ִ� , 
{
    //json���� �߿� �ռ� �����ص� ������ �о��
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
