#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

ThreadManager::ThreadManager()
{
	// Main Thread Init
	InitTLS();
}

ThreadManager::~ThreadManager()
{   
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock);
	_threads.push_back(thread([=]() {
		InitTLS();
		callback();
		DestroyTLS(); 
		}));
}

void ThreadManager::Join()
{
	for (thread& t : _threads) {
		if (t.joinable()) {
			t.join();
		}
	}
	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{
}

void ThreadManager::ProcessGlobalQueue()
{
	while (true) {
		uint64 now = ::GetTickCount64();
		if (now > LEndTickCount) {
			break;
		}
		TaskQueueRef taskQueue = GGlobalQueue->Pop();
		if (taskQueue == nullptr) {
			break;
		}
		taskQueue->Execute();
	}
}

void ThreadManager::ProcessReservedTasks()
{
	uint64 now = ::GetTickCount64();
	GTaskTimer->Distribute(now);
}
