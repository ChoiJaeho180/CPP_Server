#pragma once
#include "Rect2D.h"
#include "Vector2.h"
#include "MapDesc.h"

class MonsterSpanwer;

// 유저, NPC, AI, FSM 실제 게임 로직을 돌리는 곳
class ZoneInstance : public TaskQueue
{

public:
	ZoneInstance(const uint64 instanceId, const Vector2<int> zoneIndex, const Rect2D zoneArea, const MapDesc mapCms);
	virtual ~ZoneInstance();

	void Init();

public:
	const Rect2D&				GetArea() const { return _area; }
	ZoneInstanceRef				GetZoneInstanceRef() { return static_pointer_cast<ZoneInstance>(shared_from_this()); }
public:
	void Update();
private:
	const uint64				_instanceId;
	const Vector2<int>			_zoneIndex;
	const Rect2D				_area;
	const MapDesc				_mapCms;

private:
	Vector<Vector<GridRef>>		_grids;
	Vector<MonsterSpanwerRef>	_spawners;
};

