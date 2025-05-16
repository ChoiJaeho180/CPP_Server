#include "pch.h"
#include "GlobalQueue.h"

GlobalQueue::GlobalQueue()
{
}

GlobalQueue::~GlobalQueue()
{
}

void GlobalQueue::Push(TaskQueueRef taskQueue)
{
	_taskQueues.Push(taskQueue);
}

TaskQueueRef GlobalQueue::Pop()
{
	return _taskQueues.Pop();
}
