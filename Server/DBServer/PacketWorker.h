#pragma once
#include "DBServerPacket.h"


class PacketWorker
{
public:

	void										Tick();
	void										EnqueuePacket(DBServerPacketRef pkt);
	void										NotifyReady();
private:
	USE_LOCK;
	HashMap<uint64, Queue<DBServerPacketRef>>	_packetsByTargetId;
	bool										_isReady = false;
	std::condition_variable						_cv;
	std::mutex									_cvMutex; // condition_variable Àü¿ë
};

