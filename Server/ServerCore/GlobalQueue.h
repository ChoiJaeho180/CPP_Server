#pragma once
class GlobalQueue
{
public:
	GlobalQueue();
	~GlobalQueue();

	void Push(TaskQueueRef taskQueue);
	TaskQueueRef Pop();

private:
	LockQueue<TaskQueueRef> _taskQueues;
};

