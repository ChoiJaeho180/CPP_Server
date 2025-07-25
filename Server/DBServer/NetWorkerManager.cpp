#include "NetWorkerManager.h"
#include "DBConst.h"

NetWorkerManager::NetWorkerManager()
{

}
NetWorkerManager::~NetWorkerManager()
{
	_workers.clear();
}

void NetWorkerManager::AddWorker(int key, NetWorkerRef worker)
{
	_workers[key] = worker;
	cout << "Add Net Worker : " << key << endl;
}

void NetWorkerManager::EnqueuePacket(const uint64 targetId, SendBufferRef pkt)
{
	const uint16 shardId = targetId % DBConst::NETWORK_WORKER_COUNT;
	cout << "NetWorkerManager::EnqueuePacket : " << shardId << ", " << "targetId : " << targetId << endl;
	_workers[shardId]->EnqueuePacket(pkt);
}

