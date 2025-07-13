#pragma once
#include "PacketWorker.h"


class PacketWorker;
class SendBuffer;
struct DBServerPacket;

class PacketWorkerManager
{
public:
	static PacketWorkerManager& GetInstance() {
		static PacketWorkerManager instance;
		return instance;
	}

	PacketWorkerManager();
	~PacketWorkerManager();

	void EnqueuePacket(DBServerPacketRef pkt);
	void AddWorker(int key, PacketWorkerRef worker);
	HashMap<uint16, PacketWorkerRef>& GetAllWorkers() { return _workers; }

private:
	HashMap<uint16, PacketWorkerRef> _workers;
};


