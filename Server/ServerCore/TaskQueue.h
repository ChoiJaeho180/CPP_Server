#pragma once
#include "Task.h"
#include "LockQueue.h"
#include "TaskTimer.h"

class TaskQueue : public enable_shared_from_this<TaskQueue>
{
public:
	void DoAsync(CallbackType&& callback) {
		Push(ObjectPool<Task>::MakeShared(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memFunc)(Args ...), Args... args) {
		weak_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push( ObjectPool<Task>::MakeShared(owner, memFunc, std::forward<Args>(args)...));
	}

	void DoTimer(uint64 tickAfter, CallbackType&& callback) {
		TaskRef task = ObjectPool<Task>::MakeShared(std::move(callback));
		GTaskTimer->Reserve(tickAfter, shared_from_this(), task);
	}

	template<typename T, typename Ret, typename... Args>
	void DoTimer(uint64 tickAfter, Ret(T::* memFunc)(Args ...), Args... args) {
		weak_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		TaskRef task = ObjectPool<Task>::MakeShared(owner, memFunc, std::forward<Args>(args)...);
		GTaskTimer->Reserve(tickAfter, owner, task);
	}
	
public:
	// �� ���̵� ���� ��ü�� ���ļ� ���� + ��Ƽ������ ���� + ���� ������
	void						Push(TaskRef task, bool bPushOnly = false);
	void						Execute();
protected:
	LockQueue<TaskRef>		_tasks;
	Atomic<uint32>			_taskCount = 0;
};

