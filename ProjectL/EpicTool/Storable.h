#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <boost/pfr.hpp>
#include <type_traits>

using json = nlohmann::json;

namespace EpicTool
{
	// ���ڵ� ���ڵ��� ����

	template<typename T, typename = void>  // is_STL_Container�� ���, stl �����̳����� ���θ� Ȯ��
	class is_STL_Container
		:public std::false_type
	{

	};

	template<int N, typename T>
	json FieldPreEncoding(T& classInstance, json& data)
	{
		if constexpr (N == 0)
		{
			return data;
		}
		else
		{
			if constexpr (!std::is_pointer_v<std::remove_reference_t<decltype(boost::pfr::get<N - 1>(classInstance))>>)  // std::remove_reference_t Ÿ�Կ��� ������ ���� �ϴ� �Լ�
			{
				data[boost::pfr::get_name<N - 1, std::remove_reference_t<T>>()] = boost::pfr::get<N - 1>(classInstance);
			}
			FieldPreEncoding<N - 1, T>(classInstance, data);
			
		}
		return data;
	}

	class Storable
	{
	public:    
		json Encomding() const;

		bool Decoding(const json& data);

	protected:
		virtual bool PreEncoding(json& data) const = 0;
		virtual bool PostEncoding(json& data) const = 0;
	};

}

#define TO_JSON(Class) \
operator json() const \
{ \
	json data; \
	EpicTool::FieldPreEncoding<boost::pfr::tuple_size_v<Class>>( *this, data); \
	return data; \
} 