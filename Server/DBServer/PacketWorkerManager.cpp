#include "pch.h"
#include "PacketWorkerManager.h"
#include "DBConst.h"
#include "DBServerPacket.h"

PacketWorkerManager::PacketWorkerManager()
{
}

PacketWorkerManager::~PacketWorkerManager()
{
	_workers.clear();
}

void PacketWorkerManager::EnqueuePacket(DBServerPacketRef pkt)
{
	const uint16 shardId = pkt->header.targetId % DBConst::DB_JOB_SHARD_COUNT;
	cout << "EnqueuePacket shardId : " << shardId << ", " << "targetId : " << pkt->header.targetId << endl;
	_workers[shardId]->EnqueuePacket(pkt);
}

void PacketWorkerManager::AddWorker(int key, PacketWorkerRef worker)
{
	cout << "Add Packet Worker : " << key << endl;
	_workers[key] = worker;
}
