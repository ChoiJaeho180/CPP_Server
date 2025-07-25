#pragma once
#include "DBServerPacket.h"


class PacketWorker
{
public:
	PacketWorker();
	~PacketWorker();
	void										Init();
	void										Run();
	void										EnqueuePacket(DBServerPacketRef pkt);
	void										NotifyReady();
private:
	USE_LOCK;
	Queue<DBServerPacketRef>					_packets;

	bool										_isReady = false;
	std::condition_variable						_cv;
	std::mutex									_cvMutex; // condition_variable Àü¿ë
};

