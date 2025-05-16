#include "pch.h"
#include "TaskQueue.h"

void TaskQueue::Push(TaskRef&& task)
{
	const uint32 prevCount = _taskCount.fetch_add(1);
	_tasks.Push(task); // WRITE_LOCK;

	// _tasks 에 처음 추가한 스레드만 execute 실행
	if (prevCount == 0) {
		Execute();
	}
}

void TaskQueue::Execute()
{
	while (true) {
		Vector<TaskRef> tasks;
		_tasks.PopAll(OUT tasks);

		const uint32 taskCount = static_cast<uint32>(tasks.size());
		for (int i = 0; i < taskCount; i++) {
			tasks[i]->Execute();
		}

		if (taskCount == _taskCount.fetch_sub(taskCount)) {
			break;
		}
	}
}
