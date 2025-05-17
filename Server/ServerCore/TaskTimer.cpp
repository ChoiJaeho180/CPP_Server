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
	// �ѹ��� 1�����常 ���
	// ���� : �ؾ������� �� ������ �ʰ� ����ɰ� ���� ����� �� ����.

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
