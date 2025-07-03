#pragma once
#include "../Libraries/Include/nlohmann/json.hpp"

struct MonsterDesc  {
	int cmsId;
	std::string name;
	int level;
	int hp;
	int exp;
	int moveSpeed;
	int aggroRange;
	int attackRange;
	int dropGroupId;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MonsterDesc, cmsId, name, level, hp, exp, moveSpeed, aggroRange, attackRange, dropGroupId);