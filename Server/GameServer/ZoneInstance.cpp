#include "pch.h"
#include "ZoneInstance.h"
#include "CmsManager.h"
#include "MonsterSpawnPointDesc.h"
#include "CmsLookup.h"
#include "MonsterSpanwer.h"
#include "Grid.h"

ZoneInstance::ZoneInstance(const uint64 instanceId, const Vector2<int> zoneIndex, const Rect2D zoneArea, const MapDesc mapCms)
	: _instanceId(instanceId), _zoneIndex(zoneIndex), _area(zoneArea), _mapCms(mapCms)
{

}

ZoneInstance::~ZoneInstance()
{
}

void ZoneInstance::Init()
{
	const int gridCountY = _mapCms.zoneHeight / _mapCms.gridHeight;
	const int gridCountX = _mapCms.zoneWidth / _mapCms.gridWidth;

	_grids.resize(gridCountY);
	for (int y = 0; y < gridCountY; y++) {

		_grids[y].resize(gridCountX);
		for (int x = 0; x < gridCountX; x++) {

			Vector2<int> gridIndex = { x, y };
			const Rect2D area = Rect2D(
				_zoneIndex.x * _mapCms.zoneWidth + gridIndex.x * _mapCms.gridWidth,
				_zoneIndex.y * _mapCms.zoneHeight + gridIndex.y * _mapCms.gridHeight,
				_mapCms.gridWidth - 1,
				_mapCms.gridHeight - 1
			);
			//std::cout << "x : " << area.x << ", endX : " << area.endX <<  ", y : " << area.y <<  ", endY : " << area.endY << std::endl;
			_grids[y][x] = ObjectPool<Grid>::MakeShared(GetZoneInstanceRef(), gridIndex, area);
		}
	}
}

void ZoneInstance::Update()
{
	uint64 tick = GetTickCount64();
		
	for (const auto& spawner : _spawners) {
		spawner->Update(tick);
	}
}
