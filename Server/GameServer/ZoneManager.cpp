#include "pch.h"
#include "ZoneManager.h"
#include "Zone.h"
#include "CmsManager.h"
#include <atomic>
#include "../ServerCore/AtomicScopeGuard.h"
#include "MapDesc.h"
#include "ZoneUtils.h"

#define UPDATE_INTERVER 100

void ZoneManager::Init()
{
	
	const auto& mapCmses = CmsManager::GetAll<MapDesc>();
	for (const auto& [mapCmsId, mapCms] : mapCmses)
	{
		int zoneCountX = mapCms.width / mapCms.zoneWidth;
		int zoneCountY = mapCms.height / mapCms.zoneHeight;

		for (int y = 0; y < zoneCountY; ++y) {
			for (int x = 0; x < zoneCountX; ++x) {
				const uint64 key = ZoneUtils::MakeZoneCoordKey(x, y);
				_zones[key] = MakeShared<Zone>(key, mapCms);
			}
		}
	}
}

void ZoneManager::EnterPlayer(const Protocol::LocationYaw& pos, int mapCmsId)
{
}

void ZoneManager::EnqueueUpdates()
{
	AtomicScopeGuard<bool> guard(_isReserving, false, true, false);
	if (!guard) {
		return;
	}

	const uint64 curTick = ::GetTickCount64();
	if (_lastReservedTick + UPDATE_INTERVER > curTick) {
		return;
	}
	
	for (const auto& [_, zone] : _zones) {
		zone->EnqueueUpdates();
	}

	_lastReservedTick = curTick;
}


ZoneInstanceRef ZoneManager::GetOrCreateZoneInstance(const Protocol::LocationYaw& pos, int mapCmsId)
{
	{
		READ_LOCK;

	}

	return ZoneInstanceRef();
}
