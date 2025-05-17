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
DBConnectionPool*			GDBConnectionPool = nullptr;

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
		GDBConnectionPool = new DBConnectionPool();

		SocketUtils::Init();
	}
	~CoreGlobal() {
		delete GThreadManager;
		delete GDeadLockProfiler;
		delete GMemoryManager;
		delete GSendBufferManager;
		delete GGlobalQueue;
		delete GTaskTimer;
		delete GDBConnectionPool;

		SocketUtils::Clear();
	}
} GCoreGlobal;
