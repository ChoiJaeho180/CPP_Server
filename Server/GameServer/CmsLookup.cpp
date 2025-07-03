#include "pch.h"
#include "CmsLookup.h"
#include "CmsManager.h"
#include "MonsterSpawnPointDesc.h"

namespace CmsLookup {
	std::unordered_map<int, std::vector<MonsterSpawnPointDesc>> _mapToSpawners;
	void Build()
	{
		 auto& all = CmsManager::GetAll<MonsterSpawnPointDesc>();
		_mapToSpawners.reserve(all.size());
		
		for (const auto& [id, cms] : all) {
			_mapToSpawners[cms.mapCmsId].push_back(cms);
		}
	}

	const std::vector<MonsterSpawnPointDesc>& GetSpawnersByZoneId(int zoneCmsId)
	{
		static const std::vector<MonsterSpawnPointDesc> empty;
		auto it = _mapToSpawners.find(zoneCmsId);
		if (it == _mapToSpawners.end())
			return empty;

		return it->second;
	}
}
