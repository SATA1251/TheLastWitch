/// <summary>
/// ��ƼŬ���� ����ϴ� ���ҽ���
/// ���ҽ� �Ŵ������� �����ϱ� ���� ����
/// 
/// ����ȭ
/// </summary>

#pragma once
struct ID3D11ShaderResourceView;
struct ID3D11Buffer;

namespace ArkEngine
{
	class ParticleResource
	{
	public:
		ParticleResource(const std::string& particleName, ID3D11ShaderResourceView* texArray, ID3D11Buffer* initVB);
		~ParticleResource();

	public:
		ID3D11ShaderResourceView* GetTexArray();
		ID3D11Buffer* GetInitVB();

	public:
		void SetTexArray(ID3D11ShaderResourceView* texArray);
		void SetInitVB(ID3D11Buffer* initVB);

	private:
		std::string _particleName;
		ID3D11ShaderResourceView* _texArray;

		// drawVB�� streamVB�� ��ü���� ���� �������ϴ� �� ���� ���� ����
		ID3D11Buffer* _initVB;
	};
}

