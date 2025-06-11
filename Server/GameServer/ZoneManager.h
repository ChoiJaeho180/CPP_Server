#pragma once

class Zone;

//	입장 처리, 통계, 전체 업데이트

class ZoneManager
{
public:
	static ZoneManager& GetInstance() {
		static ZoneManager instance;
		return instance;
	}

	void							Init();
	void							EnqueueUpdates();
private:
	unordered_map<uint64, ZoneRef>	_zones;
	atomic<bool>					_isReserving;
	uint64							_lastReservedTick;
};

