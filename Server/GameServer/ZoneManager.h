#pragma once

class Zone;
class ZoneInstance;

//	���� ó��, ���, ��ü ������Ʈ
class ZoneManager
{
	USE_LOCK;

public:
	static ZoneManager& GetInstance() {
		static ZoneManager instance;
		return instance;
	}

	void							Init();
	void							EnterPlayer(const Protocol::LocationYaw& pos, int mapCmsId);
	ZoneInstanceRef					GetOrCreateZoneInstance(const Protocol::LocationYaw& pos, int mapCmsId);
public:
	void							EnqueueUpdates();
	
private:
	HashMap<uint64, ZoneRef>		_zones;
	atomic<bool>					_isReserving;
	uint64							_lastReservedTick;
};

