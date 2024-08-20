
#include "Storable.h"

namespace EpicTool
{


	json Storable::Encomding() const
	{
		json data;

		PreEncoding(data);

		return data;
	}

	bool Storable::Decoding(const json& data)
	{
		return true;
	}

}
