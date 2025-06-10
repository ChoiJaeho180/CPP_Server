#pragma once
#include "../Libraries/Include/nlohmann/json.hpp"
struct ZoneDesc  {
	int id;
	std::string name;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ZoneDesc, id, name);