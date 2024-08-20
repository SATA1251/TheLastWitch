#include <iostream>
#include <filesystem>
#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/scene.h>
#include <d3d11.h>

#include "Utils.h"
#include "AsTypes.h"
#include "tinyxml2.h"
#include "FileUtils.h"
#include "FBXConverter.h"

ArkEngine::FBXConverter::FBXConverter()
	:_material(nullptr), _scene(nullptr), _importer(nullptr),
	_assetPath(), _modelPath(), _texturePath(),
	_utils(nullptr)
{
	_importer = std::make_shared<Assimp::Importer>();

	// Resource�� ���� ��� �ʱ�ȭ
	_assetPath = L"Resources/Assets/";
	_modelPath = L"Resources/Models/";
	_texturePath = L"Resources/Textures/";
}

ArkEngine::FBXConverter::~FBXConverter()
{

}

void ArkEngine::FBXConverter::ReadAssetFile(std::wstring file)
{
	std::wstring fileStr = file;

	auto p = std::filesystem::path(fileStr);
	assert(std::filesystem::exists(p));

	_scene = _importer->ReadFile(
		_utils->ToString(fileStr),
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_FixInfacingNormals |
		aiProcess_LimitBoneWeights

	);

	assert(_scene != nullptr);
}

void ArkEngine::FBXConverter::ReadAnimAssetFile(std::wstring filePath)
{
	//std::wstring fileStr = _assetPath + filePath;


	auto p = std::filesystem::path(filePath);
	assert(std::filesystem::exists(p));

	_scene = _importer->ReadFile(
		_utils->ToString(filePath),
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_FixInfacingNormals |
		aiProcess_LimitBoneWeights

	);

	assert(_scene != nullptr);
}

void ArkEngine::FBXConverter::ExportModelData(std::wstring savePath)
{
	std::wstring finalPath = _modelPath + savePath + L".mesh";
	ReadModelData(_scene->mRootNode, 0, -1);

	/// ���� ����ó�� ������� ��Ų������ ���� FBX������ ���� ���̰�

	ReadSkinData();

	// Write CSV File
	{
		FILE* file;
		::fopen_s(&file, "../Vertices1.csv", "w");

		for (std::shared_ptr<asBone>& bone : _bones)
		{
			std::string name = bone->name;
			::fprintf(file, "%d,%s\n", bone->index, bone->name.c_str());
		}

		::fprintf(file, "\n");

		for (std::shared_ptr<asMesh>& mesh : _meshes)
		{
			std::string name = mesh->name;
			//::fprintf("%s\n", name.c_str());

			for (unsigned int i = 0; i < mesh->vertices.size(); i++)
			{
				Vec3 p = mesh->vertices[i].weight;
				//Vec3 normal = mesh->vertices[i].normal;
				//Vec2 uv = mesh->vertices[i].uv;
				//DirectX::XMUINT4 indices = mesh->vertices[i].boneIndices;
				//Vec3 weights = mesh->vertices[i].weight;

				::fprintf(file, "%f, %f, %f, \n", p.x, p.y, p.z);
				//::fprintf(file, "%f, %f, %f,", tan.x, tan.y, tan.z);
				//::fprintf(file, "%f, %f, %f,", normal.x, normal.y, normal.z);
				//::fprintf(file, "%f, %f, \n", uv.x, uv.y);

				//::fprintf(file, "%d, %d, %d, %d, ", indices.x, indices.y, indices.z, indices.w);

				//::fprintf(file, "%f, %f, %f,\n", weights.x, weights.y, weights.z);

			}
		}

		::fclose(file);
	}

	WriteModelFile(finalPath);
}

void ArkEngine::FBXConverter::ExportMaterialData(std::wstring savePath)
{
	std::wstring finalPath = _texturePath + savePath + L".xml";
	ReadMaterialData();
	WriteMaterialData(finalPath);
}

void ArkEngine::FBXConverter::ExportAnimationData(std::wstring savePath, unsigned int index)
{
	std::unique_ptr<Utils> utils = std::make_unique<Utils>();

	std::wstring finalPath = _modelPath + savePath + L".clip";
	assert(index < _scene->mNumAnimations);

	std::wstring animName = L"";

	size_t found = savePath.find(L'/');
	if (found != std::wstring::npos)
	{
		animName = savePath.substr(found + 1);
	}

	std::shared_ptr<asAnimation> animation = ReadAnimationData(_scene->mAnimations[index], utils->ToString(animName));
	WriteAnimationData(animation, finalPath);
}

void ArkEngine::FBXConverter::ReadModelData(aiNode* node, int index, int parent)
{
	/// ���汸���� �Ľ�

	// asBone
	_bone = std::make_shared<asBone>();
	_bone->index = index;
	_bone->parent = parent;
	_bone->name = node->mName.C_Str();

	// �ٷ� �� bone�� ����
	// Relative Transform
	Matrix transform(node->mTransformation[0]);
	_bone->transform = transform.Transpose();

	// Root (Local)
	Matrix matParent = Matrix::Identity;
	if (parent >= 0)
		matParent = _bones[parent]->transform;

	// ��Ʈ���� ���� ���
	// Local (Root) Transform
	_bone->transform = _bone->transform * matParent;

	_bones.push_back(_bone);

	// Mesh
	ReadMeshData(node, index);

	// ��� �Լ�
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ReadModelData(node->mChildren[i], _bones.size(), index);
	}

}

void ArkEngine::FBXConverter::ReadMeshData(aiNode* node, int bone)
{
	if (node->mNumMeshes < 1)
		return;

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		std::shared_ptr<asMesh> mesh = std::make_shared<asMesh>();
		mesh->name = node->mName.C_Str();
		mesh->boneIndex = bone;

		unsigned int index = node->mMeshes[i];
		aiMesh* srcMesh = _scene->mMeshes[index];
		mesh->mesh = srcMesh;

		// Material Name
		const aiMaterial* material = _scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->materialName = material->GetName().C_Str();

		const unsigned int startVertex = mesh->vertices.size();

		for (unsigned int v = 0; v < srcMesh->mNumVertices; v++)
		{
			// Vertex
			VertexType vertex;
			::memcpy(&vertex.position, &srcMesh->mVertices[v], sizeof(Vec3));

			// UV
			if (srcMesh->HasTextureCoords(0))
			{
				::memcpy(&vertex.uv, &srcMesh->mTextureCoords[0][v], sizeof(Vec2));
			}

			// Normal
			if (srcMesh->HasNormals())
			{
				::memcpy(&vertex.normal, &srcMesh->mNormals[v], sizeof(Vec3));
			}

			// Tangent
			if (srcMesh->HasTangentsAndBitangents())
			{
				::memcpy(&vertex.tangent, &srcMesh->mTangents[v], sizeof(Vec3));
			}

			mesh->vertices.push_back(vertex);
		}

		// Index
		for (unsigned int f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (unsigned int k = 0; k < face.mNumIndices; k++)
			{
				mesh->indices.push_back(face.mIndices[k] + startVertex);
			}
		}

		_meshes.push_back(mesh);
	}
}

void ArkEngine::FBXConverter::ReadSkinData()
{
	for (unsigned int i = 0; i < _scene->mNumMeshes; i++)
	{

		aiMesh* srcMesh = _scene->mMeshes[i];
		if (srcMesh->HasBones() == false)
			continue;

		std::shared_ptr<asMesh> mesh = _meshes[i];

		std::vector<asBoneWeights> tempVertexBoneWeights;
		tempVertexBoneWeights.resize(mesh->vertices.size());


		// Bone�� ��ȸ�ϸ鼭 ������ VertexId, Weight�� ���ؼ� ����Ѵ�
		for (unsigned int b = 0; b < srcMesh->mNumBones; b++)
		{
			// aiBone
			srcMeshBone = srcMesh->mBones[b];
			// ���� ��������
			unsigned int boneIndex = GetBoneIndex(srcMeshBone->mName.C_Str());

			// BoneOffsetTM�� �����´�
			_bone->boneOffsetTM = srcMeshBone->mOffsetMatrix;


			for (unsigned int w = 0; w < srcMeshBone->mNumWeights; w++)
			{
				unsigned int index = srcMeshBone->mWeights[w].mVertexId;
				float weight = srcMeshBone->mWeights[w].mWeight;

				tempVertexBoneWeights[index].AddWeights(boneIndex, weight);

				srcMeshBone->mOffsetMatrix[index];
			}
		}

		// ���� ��� ���
		for (unsigned int v = 0; v < tempVertexBoneWeights.size(); v++)
		{
			tempVertexBoneWeights[v].Normalize();

			asBlendWeight blendWeight = tempVertexBoneWeights[v].GetBlendWeights();
			mesh->vertices[v].boneIndices = blendWeight.indices;
			mesh->vertices[v].weight = blendWeight.weights;
		}

	}
}

void ArkEngine::FBXConverter::WriteModelFile(std::wstring finalPath)
{
	auto path = std::filesystem::path(finalPath);

	// ������ ������ �����
	std::filesystem::create_directories(path.parent_path());

	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(finalPath, FileMode::Write);

	// Bone Data
	file->Write<unsigned int>(_bones.size());
	for (std::shared_ptr<asBone>& bone : _bones)
	{
		file->Write<int>(bone->index);
		file->Write<std::string>(bone->name);
		file->Write<int>(bone->parent);
		file->Write<Matrix>(bone->transform);
		file->Write<aiMatrix4x4>(bone->boneOffsetTM);
	}

	// Mesh Data
	file->Write<unsigned int>(_meshes.size());
	for (std::shared_ptr<asMesh>& meshData : _meshes)
	{
		file->Write<std::string>(meshData->name);
		file->Write<int>(meshData->boneIndex);
		file->Write<std::string>(meshData->materialName);

		// Vertex Data
		file->Write<unsigned int>(meshData->vertices.size());
		file->Write(&meshData->vertices[0], sizeof(VertexType) * meshData->vertices.size());

		//Index Data
		file->Write<unsigned int>(meshData->indices.size());
		file->Write(&meshData->indices[0], sizeof(unsigned int) * meshData->indices.size());
	}
}

void ArkEngine::FBXConverter::ReadMaterialData()
{
	for (unsigned int i = 0; i < _scene->mNumMaterials; i++)
	{
		aiMaterial* srcMaterial = _scene->mMaterials[i];

		std::shared_ptr<asMaterial> material = std::make_shared<asMaterial>();
		material->name = srcMaterial->GetName().C_Str();

		aiColor3D color;

		// Ambient
		if (srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) 
		{
			material->ambient = Color(color.r, color.g, color.b, 1.0f);
		}

		// Diffuse
		if (srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) 
		{
			material->diffuse = Color(color.r, color.g, color.b, 1.0f);
		}

		// Specular
		if (srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) 
		{
			material->specular = Color(color.r, color.g, color.b, 1.0f);
			srcMaterial->Get(AI_MATKEY_SHININESS, material->specular.w);
		}

		// Emissive
		if (srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) 
		{
			material->emissive = Color(color.r, color.g, color.b, 1.0f);
		}

		aiString file;

		// Diffuse Texture
		if (srcMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) 
		{
			srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
			material->diffuseFile = file.C_Str();
		}

		// Specular Texture
		if (srcMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0) 
		{
			srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
			material->specularFile = file.C_Str();
		}

		// Normal Texture
		if (srcMaterial->GetTextureCount(aiTextureType_NORMALS) > 0) 
		{
			srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
			material->normalFile = file.C_Str();
		}

		// Emissive Texture
		if (srcMaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0) 
		{
			srcMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &file);
			material->emissiveFile = file.C_Str();
		}

		_materials.push_back(material);
	}
}

void ArkEngine::FBXConverter::WriteMaterialData(std::wstring finalPath)
{
	auto path = std::filesystem::path(finalPath);

	// ������ ������ �����
	std::filesystem::create_directory(path.parent_path());

	std::string folder = path.parent_path().string();

	std::shared_ptr<tinyxml2::XMLDocument> document = std::make_shared<tinyxml2::XMLDocument>();

	tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	// XML �Ľ�
	// XML ���� ���Ŀ� ���� �޶��� �� �ִ�
	tinyxml2::XMLElement* root = document->NewElement("Materials");
	document->LinkEndChild(root);

	for (std::shared_ptr<asMaterial> material : _materials)
	{
		tinyxml2::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		tinyxml2::XMLElement* element = nullptr;

		element = document->NewElement("Name");
		element->SetText(material->name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(folder, material->diffuseFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(folder, material->specularFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(folder, material->normalFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("EmissiveFile");
		element->SetText(WriteTexture(folder, material->emissiveFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->ambient.x);
		element->SetAttribute("G", material->ambient.y);
		element->SetAttribute("B", material->ambient.z);
		element->SetAttribute("A", material->ambient.w);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->diffuse.x);
		element->SetAttribute("G", material->diffuse.y);
		element->SetAttribute("B", material->diffuse.z);
		element->SetAttribute("A", material->diffuse.w);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->specular.x);
		element->SetAttribute("G", material->specular.y);
		element->SetAttribute("B", material->specular.z);
		element->SetAttribute("A", material->specular.w);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->emissive.x);
		element->SetAttribute("G", material->emissive.y);
		element->SetAttribute("B", material->emissive.z);
		element->SetAttribute("A", material->emissive.w);
		node->LinkEndChild(element);
	}

	document->SaveFile(_utils->ToString(finalPath).c_str());
}

std::string ArkEngine::FBXConverter::WriteTexture(std::string saveFolder, std::string file)
{
	std::string fileName = std::filesystem::path(file).filename().string();
	std::string folderName = std::filesystem::path(saveFolder).filename().string();

	const aiTexture* srcTexture = _scene->GetEmbeddedTexture(file.c_str());
	if (srcTexture)
	{
		std::string pathStr = (std::filesystem::path(saveFolder) / fileName).string();
		//std::string pathStr = saveFolder + fileName;

		if (srcTexture->mHeight == 0)
		{
			std::shared_ptr<ArkEngine::FileUtils> file = std::make_shared<ArkEngine::FileUtils>();
			file->Open(_utils->ToWString(pathStr), FileMode::Write);
			file->Write(srcTexture->pcData, srcTexture->mWidth);
		}
		else
		{
			//D3D11_TEXTURE2D_DESC desc;

		}
	}

	// Texture�� ����Ǿ� ���� �ʰ� ���� ������ Resources ������ �����ؼ� �־��ִ� ��
	else
	{
		std::string originStr = (std::filesystem::path(_assetPath) / folderName / file).string();
		_utils->Replace(originStr, "\\", "/");

		std::string pathStr = (std::filesystem::path(saveFolder) / fileName).string();
		_utils->Replace(pathStr, "\\", "/");

		::CopyFileA(originStr.c_str(), pathStr.c_str(), false);
	}

	return fileName;
}

std::shared_ptr<ArkEngine::asAnimation> ArkEngine::FBXConverter::ReadAnimationData(aiAnimation* srcAnimation, std::string animName)
{
	std::shared_ptr<ArkEngine::asAnimation> animation = std::make_shared<ArkEngine::asAnimation>();
	animation->name = animName;
	animation->frameRate = (float)srcAnimation->mTicksPerSecond;
	animation->frameCount = (unsigned int)srcAnimation->mDuration + 1;

	// Parsing �ٽ� �ڵ�
	std::map<std::string, std::shared_ptr<asAnimationNode>> cacheAnimNodes;;

	for (unsigned int i = 0; i < srcAnimation->mNumChannels; i++)
	{
		aiNodeAnim* srcNode = srcAnimation->mChannels[i];

		// �ִϸ��̼� ��� ������ �Ľ�
		std::shared_ptr<ArkEngine::asAnimationNode> node = ParseAnimationNode(animation, srcNode);

		// ���� ã�� ��� �߿� ���� �� �ð����� �ִϸ��̼� �ð� ����
		animation->duration = max(animation->duration, node->keyframe.back().time);

		cacheAnimNodes[srcNode->mNodeName.C_Str()] = node;
	}

	// Root ���� �����ؼ� �ϳ��� �ϳ��� ��������� 
	ReadKeyframeData(animation, _scene->mRootNode, cacheAnimNodes);

	return animation;
}

std::shared_ptr<ArkEngine::asAnimationNode> ArkEngine::FBXConverter::ParseAnimationNode(std::shared_ptr<ArkEngine::asAnimation> animation, aiNodeAnim* srcNode)
{
	std::shared_ptr<asAnimationNode> node = std::make_shared<asAnimationNode>();
	node->name = srcNode->mNodeName;

	unsigned int keyCount = max(max(srcNode->mNumPositionKeys, srcNode->mNumScalingKeys), srcNode->mNumRotationKeys);

	for (unsigned int k = 0; k < keyCount; k++)
	{
		ArkEngine::asKeyFrameData frameData;

		bool found = false;
		unsigned int t = node->keyframe.size();

		// Position
		if (::fabsf((float)srcNode->mPositionKeys[k].mTime - (float)t) <= 0.0001f)
		{
			aiVectorKey key = srcNode->mPositionKeys[k];
			frameData.time = (float)key.mTime;
			::memcpy_s(&frameData.translation, sizeof(Vec3), &key.mValue, sizeof(aiVector3D));

			found = true;
		}

		// Rotation
		if (::fabsf((float)srcNode->mRotationKeys[k].mTime - (float)t) <= 0.0001f)
		{
			aiQuatKey key = srcNode->mRotationKeys[k];
			frameData.rotation.x = key.mValue.x;
			frameData.rotation.y = key.mValue.y;
			frameData.rotation.z = key.mValue.z;
			frameData.rotation.w = key.mValue.w;

			found = true;
		}

		// Scale
		if (::fabsf((float)srcNode->mScalingKeys[k].mTime - (float)t) <= 0.0001f)
		{
			aiVectorKey key = srcNode->mScalingKeys[k];
			frameData.time = (float)key.mTime;
			::memcpy_s(&frameData.scale, sizeof(Vec3), &key.mValue, sizeof(aiVector3D));

			found = true;
		}

		if (found == true)
		{
			node->keyframe.push_back(frameData);
		}
	}

	// KeyFrame �÷��ֱ�
	if (node->keyframe.size() < animation->frameCount)
	{
		unsigned int count = animation->frameCount - node->keyframe.size();
		ArkEngine::asKeyFrameData keyFrame = node->keyframe.back();

		for (unsigned int n = 0; n < count; n++)
			node->keyframe.push_back(keyFrame);
	}

	return node;
}

void ArkEngine::FBXConverter::ReadKeyframeData(std::shared_ptr<asAnimation> animation, aiNode* srcNode, std::map<std::string, std::shared_ptr<asAnimationNode>>& cache)
{
	std::shared_ptr<asKeyFrame> keyframe = std::make_shared<asKeyFrame>();
	keyframe->boneName = srcNode->mName.C_Str();

	std::shared_ptr<asAnimationNode> findNode = cache[srcNode->mName.C_Str()];

	for (unsigned int i = 0; i < animation->frameCount; i++)
	{
		asKeyFrameData frameData;

		if (findNode == nullptr)
		{
			Matrix transform(srcNode->mTransformation[0]);
			transform = transform.Transpose();
			frameData.time = (float)i;

			// simpleMath�ȿ� ��� �ִ� �Լ��̴�
			// Vector3�� ����ϴ� ���� ���� ���Ƿ� xmfloat�� ����� ���Ѵ�
			// ��ȭ �������� ����ҷ��� ����ȯ�� �ؼ� ���� �־�� �ҵ�
			transform.Decompose(OUT frameData.scale, OUT frameData.rotation, OUT frameData.translation);
		}
		else
		{
			frameData = findNode->keyframe[i];
		}

		keyframe->transforms.push_back(frameData);
	}

	// �ִϸ��̼� Ű������ ä���
	animation->keyFrame.push_back(keyframe);

	// ��������� children ������ ��ȸ
	for (unsigned int i = 0; i < srcNode->mNumChildren; i++)
	{
		ReadKeyframeData(animation, srcNode->mChildren[i], cache);
	}
}

void ArkEngine::FBXConverter::WriteAnimationData(std::shared_ptr<asAnimation> animation, std::wstring finalPath)
{
	auto path = std::filesystem::path(finalPath);

	// ������ ������ �����
	std::filesystem::create_directory(path.parent_path());

	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(finalPath, FileMode::Write);

	// �ִϸ��̼��� �̸�
	file->Write<std::string>(animation->name);
	// �ִϸ��̼��� ���ӽð�
	file->Write<float>(animation->duration);
	// ������ �ӵ�
	file->Write<float>(animation->frameRate);
	// �������� ����
	file->Write<unsigned int>(animation->frameCount);

	// �ִϸ��̼��� Ű�������� ��
	file->Write<unsigned int>(animation->keyFrame.size());

	for (std::shared_ptr<asKeyFrame> keyframe : animation->keyFrame)
	{
		file->Write<std::string>(keyframe->boneName);

		file->Write<unsigned int>(keyframe->transforms.size());
		file->Write(&keyframe->transforms[0], sizeof(asKeyFrameData) * keyframe->transforms.size());
	}

}

unsigned int ArkEngine::FBXConverter::GetBoneIndex(const std::string& name)
{
	for (std::shared_ptr<asBone>& bone : _bones)
	{
		if (bone->name == name)
			return bone->index;
	}

	assert(false);
	return 0;
}
