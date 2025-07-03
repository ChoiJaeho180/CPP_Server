#include "pch.h"
#include "CmsManager.h"
#include "../ServerCore/JsonUtils.h"
#include "MapDesc.h"
#include "MonsterDesc.h"
#include "MonsterSpawnPointDesc.h"

void CmsManager::Init(const string& directory)
{
    CmsCached<MapDesc>::Load(directory ,"Map");
    CmsCached<MonsterDesc>::Load(directory ,"Monster");
    CmsCached<MonsterSpawnPointDesc>::Load(directory ,"MonsterSpawnPoint");
}