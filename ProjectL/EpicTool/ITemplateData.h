#pragma once

// 인스턴스화 되는 데이터 이외의 모든 데이터를 담고 있는 클래스이며
// 현재는 기본적인 데이터를 가지고 있고, 해당 클래스를 상속받아 추가적인 디테일을 확장 할 수 있음

#include "Storable.h"

#include <memory>
#include <string>


namespace EpicTool
{

	class ITemplateData : public Storable
	{

	public:

		virtual void SetDataResourceName(std::string fbxName) = 0;

	protected:
		virtual bool PreEncoding(json& data) const = 0;
		virtual bool PostEncoding(json& data) const = 0;
	};
}