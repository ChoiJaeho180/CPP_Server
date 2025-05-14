#pragma once
class ITask
{
public:
	virtual void Execute() {}
};

class HealTask : public ITask 
{
public:
	virtual void Excute() {
		
	}
public:
	uint64 _target = 0;
	uint32 _healValue = 0;
};

using TaskRef = shared_ptr<ITask>;

class TaskQueue 
{
public:
	void Push(TaskRef task) {
		WRITE_LOCK;
		_tasks.push(task);
	}

	TaskRef Pop() {
		WRITE_LOCK;
		if (_tasks.empty()) {
			return nullptr;
		}

		TaskRef result = _tasks.front();
		_tasks.pop();
		return result;
	}

private:
	USE_LOCK;
	queue<TaskRef> _tasks;
};