#pragma once

struct TaskData {
	TaskData(weak_ptr<TaskQueue> owner, TaskRef task)  : owner(owner), task(task) {

	}
	weak_ptr<TaskQueue> owner;
	TaskRef				task;
};

struct TimerItem {
	uint64 executeTick = 0;

	// priority_queue�� ���������� ��� ���簡 �Ͼ�Ƿ�,
	// taskData ��ü�� �������� �ʵ��� �� �����ͷ� ������.
	TaskData* taskData = nullptr;

	bool operator<(const TimerItem& other) const {
		return executeTick > other.executeTick;
	}
};


class TaskTimer
{
public:
	void Reserve(uint64 tickAfter, weak_ptr<TaskQueue> owner, TaskRef task);
	void Distribute(uint64 now);
	void Clear();
private:
	USE_LOCK;
	PriorotyQueue<TimerItem> _items;
	Atomic<bool> _distributing = false;
};

