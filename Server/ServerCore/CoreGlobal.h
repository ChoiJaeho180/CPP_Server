#pragma once

extern class ThreadManager*			GThreadManager;
extern class MemoryManager*			GMemoryManager;
extern class DeadLockProfiler*		GDeadLockProfiler;
extern class SendBufferManager*		GSendBufferManager;
extern class GlobalQueue*			GGlobalQueue;
extern class TaskTimer*				GTaskTimer;
extern class DBConnectionPool*		GDBConnectionPool;
extern class ConsoleLog*			GConsoleLogger;

class CoreGlobal 
{
public:
	static void Init();
	static void Destory();
	;
private:
	static bool _initialized;
};