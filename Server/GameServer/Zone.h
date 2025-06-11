#pragma once
#include "ZoneDesc.h"

// ZoneInstance Ǯ ����, ������ ���� ��å ����
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

