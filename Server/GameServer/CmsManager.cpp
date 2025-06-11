#include "pch.h"
#include "CmsManager.h"
#include "../ServerCore/JsonUtils.h"
#include "MonsterDesc.h"
#include "MonsterSpawnPointDesc.h"
#include "ZoneDesc.h"

void CmsManager::Init(const string& directory)
{
    CmsCached<MonsterDesc>::Load(directory + "/Monster.json");
    CmsCached<MonsterSpawnPointDesc>::Load(directory + "/MonsterSpawnPoint.json");
    CmsCached<ZoneDesc>::Load(directory + "/Zone.json");
}