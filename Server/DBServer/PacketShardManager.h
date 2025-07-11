#pragma once
#include "PacketWorker.h"


class PacketWorker;
class SendBuffer;
struct DBServerPacket;

class PacketShardManager
{
public:
	static PacketShardManager& GetInstance() {
		static PacketShardManager instance;
		return instance;
	}

	PacketShardManager();
	~PacketShardManager();

	void EnqueuePacket(DBServerPacketRef pkt);
	void AddPacketWorker(int key, PacketWorkerRef worker);
	HashMap<uint16, PacketWorkerRef>& GetAllWorkers() { return _workers; }

private:
	HashMap<uint16, PacketWorkerRef> _workers;
};


