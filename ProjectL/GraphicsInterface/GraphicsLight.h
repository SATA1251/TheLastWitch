#pragma once

namespace GInterface
{
	class GraphicsLight
	{
	public:
		virtual ~GraphicsLight() {};

	public:
		// 활성화 상태 가져오기
		virtual bool GetActive() abstract;
		// 활성화 상태 설정
		virtual void SetActive(bool ox) abstract;

	public:
		virtual void Delete() abstract;
	};
}