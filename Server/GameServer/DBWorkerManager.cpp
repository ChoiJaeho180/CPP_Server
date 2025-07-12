#include "pch.h"
#include "DBWorkerManager.h"
#include "GameConst.h"

DBWorkerManager::DBWorkerManager()
{
}

DBWorkerManager::~DBWorkerManager()
{
	_workers.clear();
}

void DBWorkerManager::AddWorker(const uint64 shardId, DBWorkerRef& worker)
{
	_workers[shardId] = worker;
}

void DBWorkerManager::AddDBTask(const uint64 id, SendBufferRef sendBuffer)
{
	const int shardId = GetShardId(id);
	_workers[shardId]->AddTask(sendBuffer);
}

int DBWorkerManager::GetShardId(const uint64 id)
{
	return (id % GameConst::DB_JOB_SHARD_COUNT);
}
