#pragma once
#include <stack>

extern thread_local uint32				  LThreadId;
extern thread_local uint64				  LEndTickCount;

extern thread_local std::stack<int32>	  LLockStack;
extern thread_local SendBufferChunkRef	  LSendBufferChunk;
// ���� �����尡 � taskQueue�� ������ �ִ��� Ȯ�� ��
extern thread_local class TaskQueue*      LCurrentTaskQueue;
