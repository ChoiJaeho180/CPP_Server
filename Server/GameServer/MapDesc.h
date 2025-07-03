#pragma once
#include "../Libraries/Include/nlohmann/json.hpp"
#include "CommonType.h"

struct MapDesc  {
	int cmsId;
	std::string name;
	int width;
	int height;
	int zoneWidth;
	int zoneHeight;
	int gridWidth;
	int gridHeight;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MapDesc, cmsId, name, width, height, zoneWidth, zoneHeight, gridWidth, gridHeight);