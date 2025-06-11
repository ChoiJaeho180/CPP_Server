#pragma once
#include "../Libraries/Include/nlohmann/json.hpp"

struct ZoneDesc  {
	int cmsId;
	std::string name;
	int maxInstance;
	int minInstance;
	std::vector<int> Test;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ZoneDesc, cmsId, name, maxInstance, minInstance, Test);