#include "pch.h"
#include "MonsterSpanwer.h"
#include "Monster.h"
#include "ZoneInstance.h"
#include "Rect2D.h"
#include "MathUtils.h"
#include "Vector2.h"

MonsterSpanwer::MonsterSpanwer(ZoneInstanceRef owner, const MonsterSpawnPointDesc spawnerCms)
	: _owner(owner), _spawnerCms(spawnerCms)
{
	_lastSpawnTick = 0;
	_spawnedMonsters.reserve(spawnerCms.maxCount);

	const Rect2D localPos = _spawnerCms.position;
	_spawnArea = localPos.ClampTo(owner->GetArea());
}

MonsterSpanwer::~MonsterSpanwer()
{
}

void MonsterSpanwer::Update(const uint64 tick)
{
	if (_lastSpawnTick > tick) {
		return;
	}

	TrySpawn();

	_lastSpawnTick = tick + _spawnerCms.respawnTimeMs;
}

bool MonsterSpanwer::TrySpawn()
{
	bool isAllKill = true;
	for (auto& monster : _spawnedMonsters) {
		if (auto locked = monster.lock()) {
			return false;
		}
	}

	if (auto ownerLocked = _owner.lock()) {
		for (int i = 0; i < _spawnedMonsters.size(); i++) {
			Vector2<float> position = Vector2<float>(
				MathUtils::rand(_spawnArea.x, _spawnArea.endX), 
				MathUtils::rand(_spawnArea.y, _spawnArea.endY)
				);

			MonsterRef monster = ObjectPool<Monster>::MakeShared();
			_spawnedMonsters[i] = monster;
		}
	}
	
	return false;
}
