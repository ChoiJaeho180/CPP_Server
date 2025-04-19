#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "Memory.h"
#include "SocketUtils.h"
#include "SendBuffer.h"

ThreadManager* GThreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
MemoryManager* GMemoryManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;

class CoreGlobal
{
public:
	CoreGlobal() {
		GThreadManager = new ThreadManager();
		GDeadLockProfiler = new DeadLockProfiler();
		GMemoryManager = new MemoryManager();
		GSendBufferManager = new SendBufferManager();
		SocketUtils::Init();
	}
	~CoreGlobal() {
		delete GThreadManager;
		delete GDeadLockProfiler;
		delete GMemoryManager;
		delete GSendBufferManager;
		SocketUtils::Clear();
	}
} GCoreGlobal;
