#include "pch.h"
#include "PacketShardManager.h"
#include "DBConst.h"
#include "DBServerPacket.h"

PacketShardManager::PacketShardManager()
{
}

PacketShardManager::~PacketShardManager()
{
}

void PacketShardManager::EnqueuePacket(DBServerPacketRef pkt)
{
	const uint16 shardId = pkt->header.targetId % DBConst::DB_JOB_SHARD_COUNT;
	cout << "EnqueuePacket shardId : " << shardId << ", " << "targetId : " << pkt->header.targetId << endl;
	_workers[shardId]->EnqueuePacket(pkt);
	//_workers[shardId]->
}

void PacketShardManager::AddPacketWorker(int key, PacketWorkerRef worker)
{
	cout << "Add Packet Worker : " << key << endl;
	_workers[key] = worker;
}
