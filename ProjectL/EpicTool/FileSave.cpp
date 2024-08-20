
#include "FileSave.h"
#include <vector>
#include <string.h>
#include "imgui.h"
#include "KunrealAPI.h"
#include "SceneManager.h"

EpicTool::FileSave::FileSave()
{

}

EpicTool::FileSave::~FileSave()
{

}

void EpicTool::FileSave::Initalize()
{

}

void EpicTool::FileSave::OnFileSave(const std::string& filePath)
{
    nlohmann::json jsonData;
    nlohmann::json newJsonData;


    std::string fileDirectory = GetDirectoryFromPath(filePath); // 파일 경로에서 디렉토리 부분을 추출

    std::vector<KunrealEngine::GameObject*> gameObjectlist = KunrealEngine::GetCurrentScene()->
        GetObjectList();

	if (std::ifstream(filePath).good())
	{
		std::ifstream inputFile(filePath);
		inputFile >> jsonData;
		inputFile.close();
	}


    for (const auto gameObject : gameObjectlist) // 직렬화를 한 이후
    {
      std::string objectName =  gameObject->GetObjectName();

      nlohmann::json serializedData = Serialize("Main", objectName);

      newJsonData.push_back(serializedData);
    }



    //새 데이터를 추가하거나 덮어씀
    for (size_t i = 0; i < jsonData.size(); ++i)
    {
        std::string objectNameJson = jsonData[i]["ObjectName"];

        for (size_t j = 0; j < jsonData.size(); ++j)
        {
            if (newJsonData[j]["ObjectName"] == objectNameJson)
            {
                jsonData[j] = newJsonData[i];
                break;
            }
            else if (newJsonData[j]["ObjectName"] != objectNameJson)
            {
                break;
            }
        }
    }

    std::string newFilePath = fileDirectory + "/new_file.json";

    // 파일에 저장
    std::ofstream outputFile(filePath);
    if (outputFile.is_open())
    {
        outputFile << newJsonData.dump(4);
        outputFile.close();
    }
}

std::string EpicTool::FileSave::GetDirectoryFromPath(const std::string& filePath)
{
    size_t found = filePath.find_last_of("/\\");
    if (found != std::string::npos) {
        return filePath.substr(0, found);
    }
    return "";
}

nlohmann::json EpicTool::FileSave::Serialize(std::string sceneName, std::string objectName) // 디폴트값이 없나? 값을 지정하지 않은곳은 다 터짐
{
    KunrealEngine::GameObject* ptr = KunrealEngine::GetScene(sceneName)->GetGameObject(objectName);

    nlohmann::json jsonData;

    jsonData["SceneName"] = sceneName;

    jsonData["ObjectName"] = ptr->GetObjectName();  // 엔진 이식하면 수정해야함  아래도 임시일뿐


	nlohmann::json meshRenderer;  // 메쉬데이터 입력

	meshRenderer["MeshRenderer"] = nlohmann::json::array(); // 배열로 사용
	meshRenderer["TextureRenderer"] = nlohmann::json::array();

	std::vector<std::string> meshList = KunrealEngine::GraphicsSystem::GetInstance().GetRenderableList();
	std::vector<std::string> textureList = KunrealEngine::GraphicsSystem::GetInstance().GetTextureList();

	if ((ptr->GetComponent<KunrealEngine::MeshRenderer>()) != NULL) // 지금 저장을 위해서는 모든 텍스쳐의 이름이 필요함, GetTextures를 통해 뽑아 쓰자
	{
		nlohmann::json meshJson;
		meshJson["RenderingState"] = ptr->GetComponent<KunrealEngine::MeshRenderer>()->GetRenderingState();
		meshJson["Mesh"] = ptr->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshName();
		meshRenderer["MeshRenderer"].push_back(meshJson);

		nlohmann::json textureJson;
		textureJson["Diffuse"] = ptr->GetComponent<KunrealEngine::MeshRenderer>()->GetTextureName();
		//textureJson["DiffuseList"] = ptr->GetComponent<KunrealEngine::MeshRenderer>()->GetTextures();
		textureJson["Normal"] = ptr->GetComponent<KunrealEngine::MeshRenderer>()->GetNormalName();
		meshRenderer["TextureRenderer"].push_back(textureJson);
	}

	jsonData["MeshData"] = meshRenderer;


    nlohmann::json transformData;			// 트랜스폼 정보를 저장

    transformData["position_x"] = ptr->GetComponent<KunrealEngine::Transform>()->GetPosition().x;
    transformData["position_y"] = ptr->GetComponent<KunrealEngine::Transform>()->GetPosition().y;
    transformData["position_z"] = ptr->GetComponent<KunrealEngine::Transform>()->GetPosition().z;

    transformData["rotation_x"] = ptr->GetComponent<KunrealEngine::Transform>()->GetRotation().x;
    transformData["rotation_y"] = ptr->GetComponent<KunrealEngine::Transform>()->GetRotation().y;
    transformData["rotation_z"] = ptr->GetComponent<KunrealEngine::Transform>()->GetRotation().z;

    transformData["scale_x"] = ptr->GetComponent<KunrealEngine::Transform>()->GetScale().x;
    transformData["scale_y"] = ptr->GetComponent<KunrealEngine::Transform>()->GetScale().y;
    transformData["scale_z"] = ptr->GetComponent<KunrealEngine::Transform>()->GetScale().z;

    jsonData["TransformData"] = transformData;

	

    return jsonData;
}
