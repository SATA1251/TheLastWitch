/// <summary>
/// 파티클에서 사용하는 리소스를
/// 리소스 매니저에서 관리하기 위해 래핑
/// 
/// 윤종화
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

		// drawVB와 streamVB는 객체마다 각각 만들어야하는 것 같아 넣지 않음
		ID3D11Buffer* _initVB;
	};
}

