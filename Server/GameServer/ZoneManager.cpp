#include "pch.h"
#include "ZoneManager.h"
#include "Zone.h"
#include "CmsManager.h"
#include "ZoneDesc.h"
#include <atomic>
#include "../ServerCore/AtomicScopeGuard.h"

#define UPDATE_INTERVER 100

void ZoneManager::Init()
{
	const auto& zoneDeses = CmsManager::GetAll<ZoneDesc>();

	for (const auto& [cmsId, zoneDesc] : zoneDeses) {
		ZoneRef zone = ObjectPool<Zone>::MakeShared(zoneDesc);
		_zones.insert(make_pair(cmsId, zone));
	}
}

void ZoneManager::EnqueueUpdates()
{
	AtomicScopeGuard<bool> guard(_isReserving, false, true, false);
	if (!guard) {
		return;
	}

	const uint64 curTick = ::GetTickCount64();
	if (curTick > _lastReservedTick + UPDATE_INTERVER) {
		return;
	}
	
	for (const auto& [_, zone] : _zones) {
		zone->EnqueueUpdates();
	}

	_lastReservedTick = curTick;
}
