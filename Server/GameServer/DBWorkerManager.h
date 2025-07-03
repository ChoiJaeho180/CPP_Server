#pragma once
#include "DBWorker.h"

class DBWorkerManager
{
public:
	static DBWorkerManager& GetInstance() {
		static DBWorkerManager instance;
		return instance;
	}
	void AddWorker(const uint64 id, DBWorkerRef& worker);
	void AddDBTask(const uint64 id, SendBufferRef sendBuffer);

	int GetShardId(const uint64 id);

private:
	unordered_map<int, DBWorkerRef> _workers;
};

