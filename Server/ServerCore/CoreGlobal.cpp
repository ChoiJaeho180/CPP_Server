#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "Memory.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"

ThreadManager*				GThreadManager = nullptr;
DeadLockProfiler*			GDeadLockProfiler = nullptr;
MemoryManager*				GMemoryManager = nullptr;
SendBufferManager*			GSendBufferManager = nullptr;
GlobalQueue*				GGlobalQueue = nullptr;
TaskTimer*					GTaskTimer = nullptr;

class CoreGlobal
{
public:
	CoreGlobal() {
		GThreadManager = new ThreadManager();
		GDeadLockProfiler = new DeadLockProfiler();
		GMemoryManager = new MemoryManager();
		GSendBufferManager = new SendBufferManager();
		GGlobalQueue = new GlobalQueue();
		GTaskTimer = new TaskTimer();

		SocketUtils::Init();
	}
	~CoreGlobal() {
		delete GThreadManager;
		delete GDeadLockProfiler;
		delete GMemoryManager;
		delete GSendBufferManager;
		delete GGlobalQueue;
		delete GTaskTimer;

		SocketUtils::Clear();
	}
} GCoreGlobal;
