#pragma once
#include <stack>

extern thread_local uint32				  LThreadId;
extern thread_local uint64				  LEndTickCount;

extern thread_local std::stack<int32>	  LLockStack;
extern thread_local SendBufferChunkRef	  LSendBufferChunk;
// 현재 쓰레드가 어떤 taskQueue를 가지고 있는지 확인 용
extern thread_local class TaskQueue*      LCurrentTaskQueue;
