#pragma once
#include "../Libraries/Include/nlohmann/json.hpp"
#include "CommonType.h"

struct MonsterSpawnPointDesc  {
	int cmsId;
	int zoneCmsId;
	int monsterCmsId;
	Position position;
	int radius;
	float respawnTimeSec;
	int maxCount;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MonsterSpawnPointDesc, cmsId, zoneCmsId, monsterCmsId, position, radius, respawnTimeSec, maxCount);