#pragma once
#include "../Libraries/Include/nlohmann/json.hpp"
#include "CommonType.h"

struct MonsterSpawnPointDesc  {
	int cmsId;
	int mapCmsId;
	int monsterCmsId;
	J_Rect position;
	float respawnTimeMs;
	int maxCount;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MonsterSpawnPointDesc, cmsId, mapCmsId, monsterCmsId, position, respawnTimeMs, maxCount);