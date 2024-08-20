/// <summary>
/// iD3DX11Effect��
/// IResource�� ��ӹޱ� ���� ������ Ŭ����
/// 
/// ����ȭ
/// </summary>

#pragma once

namespace DirectX
{
	struct XMFLOAT4X4;
	struct XMMATRIX;
}

namespace ArkEngine
{
	namespace ArkDX11
	{
		class Transform
		{
		public:
			Transform();
			~Transform();

		public:
			void Initialize();

			const DirectX::XMMATRIX GetTransformMatrix();
			void CaculateTransform();

			const DirectX::XMMATRIX GetTranslationMatrix() const;
			const DirectX::XMVECTOR GetTranslationVector() const;
			const DirectX::XMMATRIX GetRotatinMatrix() const;

			const DirectX::XMMATRIX GetScaleMatrix() const;

			void SetTransformMatrix(const DirectX::XMFLOAT4X4& matrix);

			void SetTranslationMatrix(float translateX = 0, float translateY = 0, float translateZ = 0);

			void SetRotationMatrix(float x = 0, float y= 0, float z = 0);

			void SetScaleMatrix(float scaleX = 1, float scaleY = 1, float scaleZ = 1);

		private:
			// x�� ȸ����� ��ȯ
			void SetXRotationMatrix(float rotationSize);
			// y�� ȸ����� ��ȯ
			void SetYRotationMatrix(float rotationSize);
			// z�� ȸ����� ��ȯ
			void SetZRotationMatrix(float rotationSize);

		private:
			DirectX::XMFLOAT4X4 _transform;

			DirectX::XMFLOAT4X4 _translation;

			DirectX::XMFLOAT4X4 _rotationX;
			DirectX::XMFLOAT4X4 _rotationY;
			DirectX::XMFLOAT4X4 _rotationZ;

			DirectX::XMFLOAT4X4 _scale;
		};
	}
}


