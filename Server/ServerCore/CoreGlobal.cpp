#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "Memory.h"
#include "SocketUtils.h"

ThreadManager* GThreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
MemoryManager* GMemoryManager = nullptr;

class CoreGlobal
{
public:
	CoreGlobal() {
		GThreadManager = new ThreadManager();
		GDeadLockProfiler = new DeadLockProfiler();
		GMemoryManager = new MemoryManager();
		SocketUtils::Init();
	}
	~CoreGlobal() {
		delete GThreadManager;
		delete GDeadLockProfiler;
		delete GMemoryManager;
		SocketUtils::Clear();
	}
} GCoreGlobal;
