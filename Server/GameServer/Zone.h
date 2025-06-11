#pragma once
#include "ZoneDesc.h"

// ZoneInstance 钱 包府, 积己苞 昏力 沥氓 包府
class Zone
{
public:
	Zone(const ZoneDesc& zoneCms);
	virtual ~Zone();
public:
	void									EnqueueUpdates();
private:
	const ZoneDesc&							_zoneCms;
	unordered_map<uint32, ZoneInstanceRef>  _instancese;
	uint32									_instanceIdGen;
};

