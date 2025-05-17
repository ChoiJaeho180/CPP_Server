#include "pch.h"
#include "TaskTimer.h"
#include "TaskQueue.h"

void TaskTimer::Reserve(uint64 tickAfter, weak_ptr<TaskQueue> owner, TaskRef task)
{
	
	TimerItem item;
	item.executeTick = ::GetTickCount64() + tickAfter;
	item.taskData = ObjectPool<TaskData>::Pop(owner, task);

	WRITE_LOCK;
	_items.push(item);
}

void TaskTimer::Distribute(uint64 now)
{
	// 한번에 1스레드만 통과
	// 이유 : 극악이지만 렉 때문에 늦게 실행될게 먼저 실행될 수 있음.

	if (_distributing.exchange(true) == true) {
		return;
	}

	Vector<TimerItem> items;
	{
		WRITE_LOCK;

		while (_items.empty() == false) {
			const TimerItem& item = _items.top();
			if (item.executeTick > now) {
				break;
			}

			items.push_back(item);
			_items.pop();
		}
	}
	
	for (TimerItem& item : items) {
		if (TaskQueueRef owner = item.taskData->owner.lock()) {
			owner->Push(item.taskData->task);
		}

		ObjectPool<TaskData>::Push(item.taskData);
	}

	_distributing.store(false);
}

void TaskTimer::Clear()
{
	WRITE_LOCK;
	
	while (_items.empty() == false) {
		const TimerItem& timerItem = _items.top();
		ObjectPool<TaskData>::Push(timerItem.taskData);
		_items.pop();
	}
}
