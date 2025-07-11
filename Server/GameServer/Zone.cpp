#include "pch.h"
#include "Zone.h"
#include "ZoneInstance.h"
#include <unordered_map>
#include "ZoneUtils.h"

Zone::Zone(const uint64 key, const MapDesc mapCms) : _key(key), _mapCms(mapCms)
{
	AddZoneInstance();
}

Zone::~Zone()
{
}

void Zone::AddZoneInstance()
{
	const uint64 instanceId = _instances.size();
	const Vector2<int> zoneIndex = ZoneUtils::GetZoneIndex(_key);
	const Rect2D zoneArea = Rect2D(
		zoneIndex.x * _mapCms.zoneWidth,
		zoneIndex.y * _mapCms.zoneHeight,
		_mapCms.zoneWidth - 1,
		_mapCms.zoneHeight - 1
	);

	//std::cout << "ZoneINstance" << std::endl;
	//std::cout << "x : " << zoneArea.x << ", endX : " << zoneArea.endX << ", y : " << zoneArea.y  << ", endY : " << zoneArea.endY << std::endl;
	ZoneInstanceRef instance = ObjectPool<ZoneInstance>::MakeShared(instanceId, zoneIndex, zoneArea,  _mapCms);
	instance->Init();
	_instances.push_back(instance);
}

void Zone::EnqueueUpdates()
{
	for (auto& instance : _instances) {
		instance->Enqueue(&ZoneInstance::Update);
	}
}

