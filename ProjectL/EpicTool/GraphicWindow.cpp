#include "imgui.h"
#include "GraphicWindow.h"
#include "KunrealAPI.h"
#include "HierarchyWindow.h"
#include <DirectXMath.h>
#include <ImGuizmo.h>

/// <summary>
/// 2024.03.14
/// 현재 기즈모 Rotation이 기능구현에 문제가 있음
/// worldTM관련인듯 하나 비중이 낮으므로 보류
/// </summary>
/// <param name="screenWidth"></param>
/// <param name="screenHeight"></param>

EpicTool::GraphicWindow::GraphicWindow(int screenWidth, int screenHeight)
	: _mosX(0), _mosY(0), _screenWidth(screenWidth), _screenHeight(screenHeight), _imageSizeX(1280), _imageSizeY(720), _imguiWindowPosition{}, _imguiWindowSize{}, _isTestRotation(false), _first(false)
	
{

}

EpicTool::GraphicWindow::GraphicWindow()
{

}

EpicTool::GraphicWindow::~GraphicWindow()
{

}

void EpicTool::GraphicWindow::ShowWindow(void* _texture, int& selectedObjectIndex)
{
	// 생성 실패
	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");

	

	ImGui::Begin("GraphicsWindow", NULL, ImGuiWindowFlags_NoMove);//, ImGuiWindowFlags_NoMove);
	
	_imguiWindowPosition = ImGui::GetWindowPos();
	_imguiWindowSize = ImGui::GetWindowSize();

	_mosX = ImGui::GetMousePos().x - ImGui::GetCursorPos().x - ImGui::GetWindowPos().x;
	_mosY = ImGui::GetMousePos().y - ImGui::GetCursorPos().y - ImGui::GetWindowPos().y;
	_mosX *= (float)_screenWidth / _imageSizeX;
	_mosY *= (float)_screenHeight / _imageSizeY;
	
	int viewportPosX = 0;
	int viewportPosY = 0;
	int viewportWidth = 1280;
	int viewportHeight = 720;

	if (_mosX >= _screenWidth)
	{
		_mosX = -1;
	}
	else if (_mosX <= 0)
	{
		_mosX = -1;
	}
	
	if (_mosY >= _screenHeight)
	{
		_mosY = -1;
	}
	else if (_mosY <= 0)
	{
		_mosY = -1;
	}

	//if (ImGui::IsWindowHovered() && !ImGui::IsMouseHoveringRect(ImVec2(0, 0), ImVec2(0, 0), false) )
	//{
	//	if (!(IMGUIZMO_NAMESPACE::IsOver()) &&	ImGui::IsMouseClicked(0))
	//	{
	//		selectedObjectIndex = -1;
	//	}
	//}

	ImVec2 imageSize(_imageSizeX, _imageSizeY); // 이미지 크기

	ImGui::Image(_texture, imageSize); // 텍스처를 이미지로 출력

	ImGuiIO& io = ImGui::GetIO();
	auto _fps = io.Framerate;

	static IMGUIZMO_NAMESPACE::OPERATION gizmoOperation(IMGUIZMO_NAMESPACE::TRANSLATE);

	if (ImGui::RadioButton("Translate", gizmoOperation == IMGUIZMO_NAMESPACE::TRANSLATE))
		gizmoOperation = IMGUIZMO_NAMESPACE::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", gizmoOperation == IMGUIZMO_NAMESPACE::ROTATE))
		gizmoOperation = IMGUIZMO_NAMESPACE::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", gizmoOperation == IMGUIZMO_NAMESPACE::SCALE))
		gizmoOperation = IMGUIZMO_NAMESPACE::SCALE;

	IMGUIZMO_NAMESPACE::SetOrthographic(false);
	IMGUIZMO_NAMESPACE::SetDrawlist();
	IMGUIZMO_NAMESPACE::SetRect(_imguiWindowPosition.x, _imguiWindowPosition.y,  _imguiWindowSize.x,  _imguiWindowSize.y);


	if (selectedObjectIndex != -1) // 선택된 오브젝트가 있을 경우
	{
		DirectX::XMFLOAT4X4 resultMatrix;
		DirectX::XMFLOAT4X4 result;

		resultMatrix = _gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->GetWorldTM();
		
		
		DirectX::XMFLOAT4X4 viewMatrix = GRAPHICS->GetViewMatrix();

		DirectX::XMFLOAT4X4 projMatrix = GRAPHICS->GetProjMatrix();

		DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&viewMatrix);
		 
		DirectX::XMMATRIX pro = DirectX::XMLoadFloat4x4(&projMatrix);

		DirectX::XMMATRIX resul = DirectX::XMLoadFloat4x4(&resultMatrix);
	

		if (gizmoOperation == IMGUIZMO_NAMESPACE::TRANSLATE)
		{
			IMGUIZMO_NAMESPACE::Manipulate(
				reinterpret_cast<const float*>(&(view)),
				reinterpret_cast<const float*>(&(pro)),
				IMGUIZMO_NAMESPACE::OPERATION::TRANSLATE,
				IMGUIZMO_NAMESPACE::MODE::WORLD,
				reinterpret_cast<float*>(&resul)
			);
		}
		else if (gizmoOperation == IMGUIZMO_NAMESPACE::ROTATE)
		{
			IMGUIZMO_NAMESPACE::Manipulate(
				reinterpret_cast<const float*>(&(view)),
				reinterpret_cast<const float*>(&(pro)),
				IMGUIZMO_NAMESPACE::OPERATION::ROTATE,
				IMGUIZMO_NAMESPACE::MODE::WORLD,
				reinterpret_cast<float*>(&resul)
			);
		}
		else if (gizmoOperation == IMGUIZMO_NAMESPACE::SCALE)
		{
			IMGUIZMO_NAMESPACE::Manipulate( 
				reinterpret_cast<const float*>(&(view)),
				reinterpret_cast<const float*>(&(pro)),
				IMGUIZMO_NAMESPACE::OPERATION::SCALE,
				IMGUIZMO_NAMESPACE::MODE::WORLD,
				reinterpret_cast<float*>(&resul)
			);
		}



		if (IMGUIZMO_NAMESPACE::IsUsing())
		{
			//DirectX::XMStoreFloat4x4(&resultMatrix, resul);

			DirectX::XMVECTOR scaleMatrix;
			DirectX::XMVECTOR rotationMatrix;
			DirectX::XMVECTOR translateMatrix;

			DirectX::XMMatrixDecompose(&scaleMatrix, &rotationMatrix, &translateMatrix, resul);

			if (gizmoOperation == IMGUIZMO_NAMESPACE::TRANSLATE)
			{
				float translateX = DirectX::XMVectorGetX(translateMatrix);
				float translateY = DirectX::XMVectorGetY(translateMatrix);
				float translateZ = DirectX::XMVectorGetZ(translateMatrix);

				if (_gameObjectlist[selectedObjectIndex]->GetParent() == nullptr)
				{
					_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->SetPosition(translateX, translateY, translateZ);
				}
				else
				{
					_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->SetPositionWithGizmo(translateX, translateY, translateZ);
				}
				
			}																								
			else if (gizmoOperation == IMGUIZMO_NAMESPACE::ROTATE)
			{
				DirectX::XMMATRIX rotationMatrixQuat = DirectX::XMMatrixRotationQuaternion(rotationMatrix);

				float rotationY = asinf(rotationMatrixQuat.r[2].m128_f32[0]);
				float rotationX = atan2f(-rotationMatrixQuat.r[2].m128_f32[1], rotationMatrixQuat.r[2].m128_f32[2]);
				float rotationZ = atan2f(-rotationMatrixQuat.r[1].m128_f32[0], rotationMatrixQuat.r[0].m128_f32[0]);

				rotationX = DirectX::XMConvertToDegrees(rotationX);
				rotationY = DirectX::XMConvertToDegrees(rotationY);
				rotationZ = DirectX::XMConvertToDegrees(rotationZ);

				// WTM 과 쿼터니언을 변경해보았음
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->SetRotation(rotationX, rotationY, rotationZ );
			
			}
			else if (gizmoOperation == IMGUIZMO_NAMESPACE::SCALE)
			{
				float scaleX = DirectX::XMVectorGetX(scaleMatrix);
				float scaleY = DirectX::XMVectorGetY(scaleMatrix);
				float scaleZ = DirectX::XMVectorGetZ(scaleMatrix);

				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->SetScale(scaleX, scaleY, scaleZ);
			}
		}


	}

	ImGui::End();
	
}


POINT EpicTool::GraphicWindow::GetMousePosition()
{
	POINT point = { _mosX, _mosY };
	return point;
}



void EpicTool::GraphicWindow::SetGameObjectList(std::vector<KunrealEngine::GameObject*>& intance)
{
	_gameObjectlist = intance;
}

void EpicTool::GraphicWindow::ShowWindow()
{
    
}

void EpicTool::GraphicWindow::ShowWindow(bool* _open, std::vector<Object>& object)
{

}
