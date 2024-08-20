#include "ASEParser.h"
#include "Animator.h"
#include "ModelAnimation.h"
#include "ParsingStructs.h"

ArkEngine::ArkDX11::Animator::Animator(ASEParser* parser, int meshCount)
	: _aseParser(parser), _meshCount(meshCount), _timer(0.0f)
{

}

ArkEngine::ArkDX11::Animator::~Animator()
{
	_rotFrame_Mesh.clear();
	_posFrame_Mesh.clear();
	_aseParser = nullptr;
}

void ArkEngine::ArkDX11::Animator::PlayAnimationOnce(float deltaTime)
{
	if (_aseParser != nullptr)
	{
		if (_timer > _aseParser->GetFinalFrame())
		{
			InitAseAnimation();
		}

		else
		{
			_timer += deltaTime;
			AseAnimation(_timer);
		}
	}

}

void ArkEngine::ArkDX11::Animator::PlayAnimationContinious(float deltaTime)
{
	if (_aseParser != nullptr)
	{
		if (_timer > _aseParser->GetFinalFrame())
		{
			StopAseAnimation();
		}

		else
		{
			_timer += deltaTime;
			AseAnimation(_timer);
		}
	}
}

void ArkEngine::ArkDX11::Animator::StopAseAnimation()
{
	_timer = 0;
	if (_aseParser != nullptr)
	{
		InitAseAnimation();
	}
}

void ArkEngine::ArkDX11::Animator::InitAseAnimation()
{
	for (auto meshIndex : _aseParser->GetMeshList())
	{
		meshIndex->_newWorldTM.at(_meshCount) = meshIndex->_originWorldTM;
		meshIndex->_localTM.at(_meshCount) = meshIndex->_originLocalTM;
		meshIndex->_slerpTM.at(_meshCount) = meshIndex->_originLocalTM;

		_posFrame_Mesh.clear();
		_rotFrame_Mesh.clear();
	}
}

void ArkEngine::ArkDX11::Animator::AseAnimation(float deltaTime)
{
	for (auto i = 0; i < _aseParser->GetMeshList().size(); i++)
	{
		auto meshIndex = _aseParser->GetMeshList()[i];

		// mesh translation
		if (_aseParser->GetMeshList()[i]->m_Animation_Pos.size() > 0)
		{
			if (meshIndex->m_Animation_Pos_Index[0] <= deltaTime &&
				_posFrame_Mesh.count(i) == 0)
			{
				Frame tempframe;
				tempframe.nowFrame = meshIndex->m_Animation_Pos_Index[0];
				tempframe.nextFrame = meshIndex->m_Animation_Pos_Index[1];
				tempframe.nextFrameIndex = 1;

				_posFrame_Mesh.insert(std::make_pair(i, tempframe));
			}

			else if (_posFrame_Mesh[i].nowFrame != _posFrame_Mesh[i].nextFrame &&
				_posFrame_Mesh[i].nextFrame <= deltaTime)
			{
				_posFrame_Mesh[i].nowFrame = _posFrame_Mesh[i].nextFrame;
				_posFrame_Mesh[i].nextFrameIndex++;

				if (_posFrame_Mesh[i].nextFrameIndex < meshIndex->m_Animation_Pos_Index.size())
				{
					_posFrame_Mesh[i].nextFrame = meshIndex->m_Animation_Pos_Index[_posFrame_Mesh[i].nextFrameIndex];
				}
			}

			else if (_posFrame_Mesh[i].nowFrame < _posFrame_Mesh[i].nextFrame)
			{
				DirectX::XMVECTOR nowVec = meshIndex->m_Animation_Pos.at(_posFrame_Mesh[i].nowFrame);
				DirectX::XMVECTOR nextVec = meshIndex->m_Animation_Pos.at(_posFrame_Mesh[i].nextFrame);

				float interval = (float)(deltaTime - _posFrame_Mesh[i].nowFrame) / (float)(_posFrame_Mesh[i].nextFrame - _posFrame_Mesh[i].nowFrame);

				DirectX::XMFLOAT3 slerpVec;
				DirectX::XMMATRIX myLocal = DirectX::XMLoadFloat4x4(&meshIndex->_localTM.at(_meshCount));
				DirectX::XMStoreFloat3(&slerpVec, DirectX::XMQuaternionSlerp(nowVec, nextVec, interval));
				myLocal.r[3] = DirectX::XMVectorSet(slerpVec.x, slerpVec.y, slerpVec.z, 1.0f);
				DirectX::XMFLOAT4X4 finalLocal;
				DirectX::XMStoreFloat4x4(&finalLocal, myLocal);
				meshIndex->_localTM.at(_meshCount) = finalLocal;

				_aseParser->SetSpecificNewWorldTM(meshIndex, _meshCount);
			}
		}

		// mesh rotation
		if (meshIndex->m_Animation_Rot.size() > 0)
		{
			if (meshIndex->m_Animation_Rot_Index[0] <= deltaTime &&
				_rotFrame_Mesh.count(i) == 0)
			{
				Frame tempframe;
				tempframe.nowFrame = meshIndex->m_Animation_Rot_Index[0];
				tempframe.nextFrame = meshIndex->m_Animation_Rot_Index[1];
				tempframe.nextFrameIndex = 1;

				_rotFrame_Mesh.insert(std::make_pair(i, tempframe));
			}

			else if (_rotFrame_Mesh[i].nowFrame != _rotFrame_Mesh[i].nextFrame &&
				_rotFrame_Mesh[i].nextFrame < deltaTime)
			{
				_rotFrame_Mesh[i].nowFrame = _rotFrame_Mesh[i].nextFrame;
				_rotFrame_Mesh[i].nextFrameIndex++;

				if (_rotFrame_Mesh[i].nextFrameIndex < meshIndex->m_Animation_Rot_Index.size())
				{
					_rotFrame_Mesh[i].nextFrame = meshIndex->m_Animation_Rot_Index[_rotFrame_Mesh[i].nextFrameIndex];

					DirectX::XMMATRIX myLocal = DirectX::XMLoadFloat4x4(&meshIndex->_localTM.at(_meshCount));
					DirectX::XMVECTOR scale, rotation, translation;

					DirectX::XMMatrixDecompose(&scale, &rotation, &translation, myLocal);

					DirectX::XMMATRIX scaleMatirx = DirectX::XMMatrixScalingFromVector(scale);

					DirectX::XMMATRIX translationMatirx = DirectX::XMMatrixTranslationFromVector(translation);

					DirectX::XMMATRIX prevRotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);

					DirectX::XMVECTOR tempNowVec = meshIndex->m_Animation_Rot.at(_rotFrame_Mesh[i].nowFrame);

					DirectX::XMVECTOR nowVec = DirectX::XMQuaternionRotationAxis(tempNowVec, DirectX::XMVectorGetW(tempNowVec));

					DirectX::XMMATRIX newRotationMatrix = DirectX::XMMatrixRotationQuaternion(nowVec);

					DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixMultiply(prevRotationMatrix, newRotationMatrix);

					DirectX::XMMATRIX resultMatrix = DirectX::XMMatrixMultiply(scaleMatirx, rotationMatrix);
					resultMatrix = DirectX::XMMatrixMultiply(resultMatrix, translationMatirx);
					DirectX::XMStoreFloat4x4(&meshIndex->_localTM.at(_meshCount), resultMatrix);
				}
			}

			if (_rotFrame_Mesh[i].nowFrame < _rotFrame_Mesh[i].nextFrame)
			{
				if (deltaTime > meshIndex->m_Animation_Rot_Index[0])
				{
					DirectX::XMMATRIX myLocal = DirectX::XMLoadFloat4x4(&meshIndex->_localTM.at(_meshCount));
					DirectX::XMVECTOR scale, rotation, translation;

					DirectX::XMMatrixDecompose(&scale, &rotation, &translation, myLocal);

					DirectX::XMMATRIX scaleMatirx = DirectX::XMMatrixScalingFromVector(scale);

					DirectX::XMMATRIX translationMatirx = DirectX::XMMatrixTranslationFromVector(translation);

					DirectX::XMMATRIX prevRotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);

					float interval = (float)(deltaTime - _rotFrame_Mesh[i].nowFrame) / (float)(_rotFrame_Mesh[i].nextFrame - _rotFrame_Mesh[i].nowFrame);

					DirectX::XMVECTOR nowVec = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

					DirectX::XMVECTOR tempNextVec = meshIndex->m_Animation_Rot.at(_rotFrame_Mesh[i].nextFrame);

					DirectX::XMVECTOR nextVec = DirectX::XMQuaternionRotationAxis(tempNextVec, DirectX::XMVectorGetW(tempNextVec));

					DirectX::XMVECTOR slerpVec = DirectX::XMQuaternionSlerp(nowVec, nextVec, interval);

					DirectX::XMMATRIX newRotationMatrix = DirectX::XMMatrixRotationQuaternion(slerpVec);

					DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixMultiply(prevRotationMatrix, newRotationMatrix);

					DirectX::XMMATRIX resultMatrix = DirectX::XMMatrixMultiply(scaleMatirx, rotationMatrix);
					resultMatrix = DirectX::XMMatrixMultiply(resultMatrix, translationMatirx);
					DirectX::XMStoreFloat4x4(&meshIndex->_slerpTM.at(_meshCount), resultMatrix);

					_aseParser->SetSpecificSlerpTM(meshIndex, _meshCount);
				}
			}
		}
	}
}
