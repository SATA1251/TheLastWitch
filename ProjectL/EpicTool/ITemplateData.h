#pragma once

// �ν��Ͻ�ȭ �Ǵ� ������ �̿��� ��� �����͸� ��� �ִ� Ŭ�����̸�
// ����� �⺻���� �����͸� ������ �ְ�, �ش� Ŭ������ ��ӹ޾� �߰����� �������� Ȯ�� �� �� ����

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