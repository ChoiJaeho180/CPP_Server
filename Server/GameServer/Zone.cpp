#include "pch.h"
#include "Zone.h"
#include "ZoneInstance.h"
#include <unordered_map>

Zone::Zone(const ZoneDesc& zoneCms):_zoneCms(zoneCms)
{
	_instanceIdGen = 0;

	if (_zoneCms.minInstance) {
		for (int i = 1; i <= _zoneCms.minInstance; i++) {
			ZoneInstanceRef instance = ObjectPool<ZoneInstance>::MakeShared();
			_instancese.insert(make_pair(++_instanceIdGen, instance));
		}
	}
}

Zone::~Zone()
{
}

void Zone::EnqueueUpdates()
{
	for (auto& [_, instance] : _instancese) {
		instance->Enqueue(&ZoneInstance::Update);
	}
}

