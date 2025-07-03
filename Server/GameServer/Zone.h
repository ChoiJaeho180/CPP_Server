#pragma once
#include "MapDesc.h"

// ZoneInstance Ǯ ����, ������ ���� ��å ����
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

