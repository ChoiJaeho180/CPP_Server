#pragma once

struct TaskData {
	TaskData(weak_ptr<TaskQueue> owner, TaskRef task)  : owner(owner), task(task) {

	}
	weak_ptr<TaskQueue> owner;
	TaskRef				task;
};

struct TimerItem {
	uint64 executeTick = 0;

	// priority_queue는 내부적으로 요소 복사가 일어나므로,
	// taskData 전체를 복사하지 않도록 생 포인터로 저장함.
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

