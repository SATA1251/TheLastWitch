/// <summary>
/// �׷��Ƚ��� ���Ǵ� �پ��� ���ҽ����� ��ӹ��� �������̽�
/// ���ҽ��Ŵ����� �����ų ����
/// 
/// ����ȭ
/// </summary>

#pragma once

namespace ArkEngine
{
	class IResource abstract
	{
	public:
		IResource() {};
		virtual ~IResource() {};

	public:
		virtual void Load(const char* fileName) abstract;
		virtual void UnLoad(const char* fileName) abstract;
	};
}