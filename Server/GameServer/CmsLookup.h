#pragma once

struct MonsterSpawnPointDesc; 

namespace CmsLookup
{
    void Build();

    
    const std::vector<MonsterSpawnPointDesc>& GetSpawnersByZoneId(int zoneId);
}