#pragma once

class Zone;

class ZoneManager
{
public:
	static ZoneManager& GetInstance() {
		static ZoneManager instance;
		return instance;
	}

	void Init();
	void ReserveUpdated();
private:
	Vector<ZoneRef> _zones;

};

