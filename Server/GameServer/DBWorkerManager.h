#pragma once
#include "DBWorker.h"

class DBWorkerManager
{
public:
	DBWorkerManager();
	~DBWorkerManager();

	static DBWorkerManager& GetInstance() {
		static DBWorkerManager instance;
		return instance;
	}
	void									AddWorker(const uint64 shardId, DBWorkerRef worker);
	void									AddDBTask(const uint64 id, SendBufferRef sendBuffer);

	int										GetShardId(const uint64 id);

private:
	HashMap<int, DBWorkerRef>				_workers;
};

