#pragma once
#include "MapDesc.h"

// ZoneInstance 钱 包府, 积己苞 昏力 沥氓 包府
class Zone
{
public:
	Zone(const uint64 key, const MapDesc mapCms);
	virtual ~Zone();

public:
	void											AddZoneInstance();
public:
	void											EnqueueUpdates();
private:
	const uint64									_key;
	const MapDesc									_mapCms;
private:
	HashMap<uint64, Vector<ZoneInstanceRef>>		_instances;
};

