#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "Memory.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"

ThreadManager*					GThreadManager = nullptr;
DeadLockProfiler*				GDeadLockProfiler = nullptr;
MemoryManager*					GMemoryManager = nullptr;
SendBufferManager*				GSendBufferManager = nullptr;
GlobalQueue*					GGlobalQueue = nullptr;
TaskTimer*						GTaskTimer = nullptr;
DBConnectionPool*				GDBConnectionPool = nullptr;
ConsoleLog*						GConsoleLogger = nullptr;
bool CoreGlobal::				_initialized = false;

void CoreGlobal::Init() {
	if (_initialized) {
		return;
	}

	GThreadManager = new ThreadManager();
	GMemoryManager = new MemoryManager();
	GDeadLockProfiler = new DeadLockProfiler();
	GSendBufferManager = new SendBufferManager();
	GGlobalQueue = new GlobalQueue();
	GTaskTimer = new TaskTimer();
	GDBConnectionPool = new DBConnectionPool();
	GConsoleLogger = new ConsoleLog();

	SocketUtils::Init();
}
void CoreGlobal::Destory()
{
	if (!_initialized) {
		return;
	}

	delete GThreadManager;
	delete GDeadLockProfiler;
	delete GMemoryManager;
	delete GSendBufferManager;
	delete GGlobalQueue;
	delete GTaskTimer;
	delete GDBConnectionPool;
	delete GConsoleLogger;

	SocketUtils::Clear();
}
