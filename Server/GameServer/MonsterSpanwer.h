#pragma once
#include "MonsterSpawnPointDesc.h"
#include "Rect2D.h"

class ZoneInstance;
class Monster;

class MonsterSpanwer : public enable_shared_from_this<MonsterSpanwer>
{
public:
	MonsterSpanwer(ZoneInstanceRef instance, const MonsterSpawnPointDesc spawnerCms);
	
	~MonsterSpanwer();

public:
	void								Update(const uint64 tick);
	bool								TrySpawn();
private:
	const MonsterSpawnPointDesc			_spawnerCms;
	uint64								_lastSpawnTick;
	Rect2D								_spawnArea;
private:
	weak_ptr<ZoneInstance>				_owner;
	Vector<weak_ptr<Monster>>			_spawnedMonsters;
};
