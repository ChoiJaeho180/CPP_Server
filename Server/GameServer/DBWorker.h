#pragma once
#include "../ServerCore/LockQueue.h"

class DBWorker
{

public:
	DBWorker(int shardId);

public:
	void						AddTask(SendBufferRef task);
	void						Tick(ClientServiceRef& service);
private:
	int							_shardId;
	LockQueue<SendBufferRef>	_tasks;


	bool						_isReady = false;
	std::condition_variable		_cv;
	std::mutex					_cvMutex; // condition_variable Àü¿ë
};

using DBWorkerRef = shared_ptr<DBWorker>;

