#pragma once

namespace GInterface
{
	class GraphicsLight
	{
	public:
		virtual ~GraphicsLight() {};

	public:
		// Ȱ��ȭ ���� ��������
		virtual bool GetActive() abstract;
		// Ȱ��ȭ ���� ����
		virtual void SetActive(bool ox) abstract;

	public:
		virtual void Delete() abstract;
	};
}