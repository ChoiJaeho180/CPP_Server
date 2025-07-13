#pragma once
#include "NetWorker.h"


class NetWorkerManager
{
public:
	NetWorkerManager();
	~NetWorkerManager();

	static NetWorkerManager& GetInstance() {
		static NetWorkerManager instance;
		return instance;
	}

	void EnqueuePacket(const uint64 targetId, SendBufferRef pkt);
	void AddWorker(int key, NetWorkerRef worker);

private:
	HashMap<uint16, NetWorkerRef> _workers;
};

